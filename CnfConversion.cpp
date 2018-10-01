/*
 * CnfConversion.cpp
 *
 *  Created on: 30-Sep-2018
 *      Author: gaurav
 */
#include <iostream>
#include <stack>
#include <cstring>
#include <vector>
using namespace std;

class Expr
{
public:
	int atom;
	Expr* opd1;
	Expr* opd2;
	char oper;
	Expr(){
		this->atom = 0;
		this->opd1 = NULL;
		this->opd2 = NULL;
		this->oper = '\0';
	}
	Expr(int atom){
		this->atom = atom;
		this->opd1 = NULL;
		this->opd2 = NULL;
		this->oper = '\0';
	}
	Expr(Expr* opd1,char oper){
		this->atom = 0;
		this->opd1 = opd1;
		this->opd2 = NULL;
		this->oper = oper;
	}
	Expr(Expr* opd1, Expr* opd2, char oper){
		this->atom = 0;
		this->opd1 = opd1;
		this->opd2 = opd2;
		this->oper = oper;
	}
	bool isAtomic(){
		if(this->atom != 0 || (this->oper == '-' && this->opd1->atom !=0)){
			return true;
		}
		return false;
	}
	bool isEmpty(){
		if(this->atom == 0 && this->opd1 == NULL && this->opd2 == NULL && this->oper == '\0'){
			return true;
		}
		return false;
	}

	static char* intToString(int x){
		char* number = NULL;
		if(x==0){
			number = new char[10];
			number[0]='\0';
			return number;
		}
		number = intToString(x/10);
		x = x%10;
		char* digit= new char[2];
		digit[0] = x+'0';
		digit[1] = '\0';
		strcat(number, digit);
		//free Memory
		delete[] digit;
		digit = NULL;
		return number;
	}

	string getCNForm(){
		
		string result="";
		
		if(atom != 0){
			//cout << "Atom = " << atom << endl;
			return string(intToString(atom));
		}
		if(oper == '+' || oper == '.') {
			
			//cout << "entered oper is +.\ngetting next cnf form for opd1\n";
			
			result = opd1->getCNForm();
			
			//cout << "entered oper is +.\ngot done with next cnf form for opd1\n";
			//cout << "entered oper is +.\ngetting next cnf form for opd2\n";
			//cout << "opd2 = " << opd2 << endl;
			
			string temp2 = opd2->getCNForm();
			
			//cout << "entered oper is +.\ngot done with next cnf form for opd2\n";
			
			//cout << "cretaed op with size 2\n";
			result.push_back(oper);
			result = result + temp2;
		}
		else cout << "The operator is not + . or - !!\n";

		return result;

	}
};

class CreateExpression{
public:
	static bool isdigit(char x){
		if(x>='0' && x<='9'){
			return true;
		}
		return false;
	}
	static bool isLeftAssociativeOper(char op){
		switch(op){
		case '+':
		case '.': return true;
		default : return false;
		}
	}
	static bool isRightAssociativeOper(char op){
		switch(op){
		case '<':
		case '=':
		case '-': return true;
		default : return false;
		}

	}
	static int precedence(char op){
		if(op == '-')
			return 5;
		else if(op == '.')
			return 4;
		else if(op == '+')
			return 3;
		else if(op == '<')
			return 2;
		else if(op == '=')
			return 1;
		else
			return 0;
	}
	static void insert(char oper, stack<Expr*> &exp){
		Expr *opd1=NULL,*opd2=NULL;
		if(oper == '-'){
			opd1 = exp.top();
			exp.pop();
			exp.push(new Expr(opd1,oper));
		}
		else{
			opd2 = exp.top();
			exp.pop();
			opd1 = exp.top();
			exp.pop();
			exp.push(new Expr(opd1, opd2, oper));
		}
	}
	static Expr* createExpr(char* infix){
		stack<Expr*> exp;
		stack<char> opr;
		char oper;
		for(int i=0;infix[i]!='\0';i++){

			if(infix[i]==' ')
				continue;
			if(infix[i]=='('){
				opr.push(infix[i]);
			}
			else if(isLeftAssociativeOper(infix[i])){
				while(!opr.empty() && precedence(infix[i])<=precedence(opr.top())){
					oper = opr.top();
					opr.pop();
					insert(oper,exp);
				}
				opr.push(infix[i]);
			}
			else if(isRightAssociativeOper(infix[i])){
				while(!opr.empty() && precedence(infix[i])<precedence(opr.top())){
					oper = opr.top();
					opr.pop();
					insert(oper,exp);
				}
				opr.push(infix[i]);
			}
			else if(infix[i]==')'){
				while(!opr.empty() && (oper=opr.top())!='('){
					opr.pop();
					insert(oper,exp);
				}
				opr.pop();
			}
			else if(isdigit(infix[i])){
				int val = 0;
				while(infix[i]!='\0' &&  isdigit(infix[i])){
					val = (val*10) + (infix[i]-'0');
					i++;
				}
				i--;
				exp.push(new Expr(val));
			}
		}
		while(!opr.empty()){
			char oper = opr.top();
			opr.pop();
			insert(oper,exp);
		}
		return exp.top();
	}
};

class ConvertingRules{
public:
	static void applyRule(Expr** exprPTR){
		Expr* expr = *exprPTR;
		switch(expr->oper){
		case '<':
			expr->opd1 = new Expr(expr->opd1,'-');
			expr->oper = '+';
			break;
		case '=':
			expr->opd1 = new Expr(expr->opd1,new Expr(expr->opd2,'-'),'+');
			expr->opd2 = new Expr(new Expr(expr->opd1,'-'),expr->opd2,'+');
			expr->oper = '.';
			break;
		case '-':
			Expr* opd = expr->opd1;
			switch(opd->oper){
			case '.':
				expr->opd1 =new Expr(opd->opd1,'-');
				expr->opd2 =new Expr(opd->opd2,'-');
				expr->oper = '+';
				break;
			case '+':
				expr->opd1 =new Expr(opd->opd1,'-');
				expr->opd2 =new Expr(opd->opd2,'-');
				expr->oper = '.';
				break;
			case '<':
				expr->opd1 = opd->opd1;
				expr->opd2 = new Expr(opd->opd2,'-');
				expr->oper = '.';
				break;
			case '=':
				expr->opd1 = new Expr(opd->opd1,opd->opd2,'+');
				expr->opd2 = new Expr(new Expr(opd->opd1,'-'),new Expr(opd->opd2,'-'),'+');
				expr->oper = '.';
				break;
			case '-':
				//Memory Leaked.. ->will handle later
				*exprPTR = opd->opd1;
				break;
			}
		}
		expr = NULL;
	}

	static void distributiveRule(Expr* expr){
		if(expr!=NULL && expr->oper == '+'){
			Expr* opd = expr->opd1;
			if(opd->oper == '.'){
				expr->opd1 = new Expr(opd->opd1,expr->opd2,'+');
				expr->opd2 =new Expr(opd->opd2,expr->opd2,'+');
				expr->oper = '.';
				return;
			}
			opd = expr->opd2;
			Expr* opd1 = expr->opd1;
			if(opd->oper == '.'){
				expr->opd1 = new Expr(opd1,opd->opd1,'+');
				expr->opd2 =new Expr(opd1,opd->opd2,'+');
				expr->oper = '.';
				opd1 = NULL;
				return;
			}
		}
	}

	//Top Down
	static void CNForm(Expr** expressionPTR){
		Expr* expression = *expressionPTR;
		if(expression!=NULL){
			applyRule(expressionPTR);
			if(expression->opd1!=NULL){
				CNForm(&expression->opd1);
			}
			if(expression->opd2!=NULL){
				CNForm(&expression->opd2);
			}
		}
		expression = NULL;
	}

	static void applyDistributiveRule(Expr* expr){
		if(expr!=NULL){
			//cout << "Beofre dist rule\n";
			distributiveRule(expr);
			//cout << "After dist rule\n";
			if(expr->opd1!=NULL){
				//cout << "First not null\n";
				applyDistributiveRule(expr->opd1);
				//cout << "First done\n";
			}
			if(expr->opd2!=NULL){
				//cout << "second not null\n";
				applyDistributiveRule(expr->opd2);
				//cout << "second done\n";
			}
		}
	}

	static vector<char*> tokenize(char* str, const char* delimiter){
		vector<char*> tokens;

		char* token = strtok((char*)str,delimiter);

		while(token!=NULL){
			tokens.push_back(token);
			token = strtok(NULL,delimiter);
		}
		return tokens;
	}
	static void freeMemory(Expr* expression){
		if(expression == NULL)
			return;
		if(expression->opd1 != NULL){
			freeMemory(expression->opd1);
		}
		if(expression->opd2 != NULL){
			freeMemory(expression->opd2);
		}
		if(expression->opd1 == NULL && expression->opd2 == NULL){
			delete expression;
			expression = NULL;
		}
	}

	static void replace(char* str, char old, char newOne)
	{
		int i=0;
		while(*(str+i)!='\0')
		{
			if(*(str+i)==old){
				str[i]=newOne;
			}
			i++;
		}
	}
	static void getCNF(char* dnf, vector<char*>& clauses){
		//clear clauses
		clauses.clear();
		Expr* expression = CreateExpression::createExpr(dnf);
		
		//cout << "Expression created\n";

		CNForm(&expression);
		
		//cout << "convereted to CNF\n";
		
		applyDistributiveRule(expression);

		//cout << "distributiveRule applied\n";
		//cout << "Get CNF FORM\n";
		
		string exp = expression->getCNForm();

		//cout << "Got CNF FORM\n";

		unsigned int len = exp.length();
		char res[len+1];
		for(unsigned int i=0;i<len;i++){
			res[i]=exp[i];
		}
		res[len]='\0';
		vector<char*> terms = tokenize(res,".");
		for(unsigned int i=0;i<terms.size();i++){
			replace(terms[i],'+',' ');
			char* clause = new char[len];
			strcpy(clause,terms[i]);
			clauses.push_back(clause);
		}

		
		//freeMemory(expression);
	}
};



