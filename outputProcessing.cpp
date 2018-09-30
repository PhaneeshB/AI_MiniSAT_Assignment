#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(int argc, char* argv[]){
	
	// assume test.satoutputis available for processing
	ifstream in;
	in.open("test.satoutput");
	ofstream out;
	out.open("output.Subgraphs");
	// in.open(argv[1]);
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
					str.append(to_string(j+1) + " ");
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