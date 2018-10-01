/*
 * outputProcessing.cpp
 *
 *  Created on: 01-Oct-2018
 *      Author: gaurav
 */

#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;

char* intToString(int x){
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

int main(int argc, char* argv[]){

	// getting File names
	int len = strlen(argv[1]);
	char inFilePath[len+10];
	char outFilePath[len+10];
	strcpy(inFilePath,argv[1]);
	strcat(inFilePath,".satoutput");
	strcpy(outFilePath,argv[1]);
	strcat(outFilePath,".subgraphs");

	ifstream in;
	in.open(inFilePath);
	ofstream out;
	out.open(outFilePath);

	string s;
	in >> s;
	int var;
	int n, m, k;
	ifstream graph;
	graph.open("test.graph");
	graph >> n >> m >> k;
	bool truth[n][k];
	if( s.compare("SAT") == 0){
		//Assuming SAT will give value assignment for all the variables.
		in >> var;
		//cout << "Value of Var = " << var << endl;
		for(int i =0; i < n; i++){
			for(int j =0 ; j<k; j++){
				if(var > 0)
					truth[i][j] = true;
				else if(var < 0)
					truth[i][j] = false;
				else break; // should only never happen
				in >> var;
				//cout << "Value of Var = " << var << endl;
			}
		}
		int count;
		string str;
		for(int i = 0; i < k; i++){
			count = 0;
			str = "";
			for(int j = 0; j < n; j++){
				if(truth[j][i]){
					count++;
					string s;
					str.append(string(intToString(j+1)) + " ");
				}
			}
			if(count > 0){
				out << "#" << i+1 << " " << count << endl;
				out << str << endl;
			}
		}
	}
	else out << 0;
	out.close();
	graph.close();
	in.close();
	return 0;
}

