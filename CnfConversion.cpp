/*
 * CnfConversion.cpp
 *
 *  Created on: 30-Sep-2018
 *      Author: gaurav
 */

/*
 * expression.cpp
 *
 *  Created on: 07-Sep-2018
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

	char* getCNForm(){
		char* result = NULL;
		if(atom != 0){
			return intToString(atom);
		}
		if(oper == '-'){
			char* temp = opd1->getCNForm();
			result = new char[500];
			char op[2]={oper,'\0'};
			result[0]= '\0';
			strcat(result,op);
			strcat(result,temp);
			//free Memory
			delete[] temp;
			temp = NULL;
		}
		else if(oper != '\0') {
			char* temp1= opd1->getCNForm();
			char* temp2 = opd2->getCNForm();
			result = new char[500];
			char op[2] = {oper,'\0'};
			result[0]= '\0';
			strcat(result,temp1);
			strcat(result,op);
			strcat(result,temp2);
			//free Memory
			delete[] temp1;
			temp1 = NULL;
			delete[] temp2;
			temp2 = NULL;
		}
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
		}
		if(expression->opd1!=NULL){
			CNForm(&expression->opd1);
		}
		if(expression->opd2!=NULL){
			CNForm(&expression->opd2);
		}
		expression = NULL;
	}

	static void applyDistributiveRule(Expr* expr){
		if(expr!=NULL){
			distributiveRule(expr);
		}
		if(expr->opd1!=NULL){
			applyDistributiveRule(expr->opd1);
		}
		if(expr->opd2!=NULL){
			applyDistributiveRule(expr->opd2);
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
		if(expression->opd1 ==NULL && expression->opd2 ==NULL){
			delete expression;
			expression = NULL;
		}
	}

	static char* replace(char* str, char old, char newOne)
	{
		char* res= new char[strlen(str)+1];
		int i=0;
		while(*str!='\0')
		{
			if(*str==old){
				res[i]=newOne;
			}
			else{
				res[i]=*str;
			}
			str++;
			i++;
		}
		res[i]='\0';
		return res;
	}
	static void getCNF(char* dnf, vector<char*>& clauses){
		Expr* expression = CreateExpression::createExpr(dnf);

		CNForm(&expression);
		applyDistributiveRule(expression);
		char* res = expression->getCNForm();
		vector<char*> terms = tokenize(res,".");
		for(unsigned int i=0;i<terms.size();i++){
			clauses.push_back(replace(terms[i],'+',' '));
		}

		//Free Memory
		delete[] res;
		res= NULL;
		//freeMemory(expression);
	}
};



