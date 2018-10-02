#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>

using namespace std;

typedef pair<int,int> iPair;

class Utility{
public:
	static string intToString(int n){
		stringstream ss;
		ss<<n;
		return ss.str();
	}
};

class DrugConnect{
public:
	unsigned int n; //vertices count
	unsigned int m; //edges count
	unsigned int k; //cliques
	iPair* edges;
	int** adjacency;
	ofstream fout;

	int VnCtoVr(int vertex,int component);
	int EnCtoVr(int edge, int component);
	void addVertexExistenceClauses();
	void addEdgeExistenceClauses();
	void addCliqueExistenceClauses();
	void addVertexPairExistenceClauses();
	void printEqCluases(int a, int b, int c, int type);
	void addMutualExclusionConstraint(int&);
	void addEdgeToVertexClauses();

	DrugConnect(unsigned int n,iPair* edges,unsigned int m,unsigned int k,char* filePath){
		this->n = n;
		this->m = m;
		this->k =k;
		this->edges = edges;

		unsigned int i;
		//initialize adjacency matrix
		adjacency = new int* [this->n];
		for(i=0;i<this->n;i++){
			adjacency[i] = new int[this->n];
			memset(adjacency[i], -1, this->n*sizeof(int));
		}

		//edges are numbered 0 based
		pair<int,int> edge;
		for(i=0;i<this->m;i++){
			edge = edges[i];
			adjacency[edge.first][edge.second] = i;
			adjacency[edge.second][edge.first] = i;
		}

		fout.open(filePath, ios::out);
		fout<<"p cnf "<<endl;
	}
	~DrugConnect(){
		unsigned int i;
		//Free Adjacency
		for(i=0;i<this->n;i++){
			delete[] adjacency[i];
			adjacency[i] = NULL;
		}
		delete[] adjacency;
		adjacency = NULL;

		delete[] edges;
		edges = NULL;
		fout.close();
	}
};

int DrugConnect::VnCtoVr(int vertex,int component){
	return this->k * vertex + component + 1;
}

int DrugConnect::EnCtoVr(int edge, int component){
	int offset = this->n * this->k;
	return offset + this->k * edge + component + 1;
}

void DrugConnect::addVertexExistenceClauses(){
	//max 10 digit variable allowed
	unsigned int vertex,comp;
	for(vertex=0; vertex<this->n;vertex++){
		for(comp=0;comp<this->k;comp++){
			fout<<VnCtoVr(vertex,comp)<<" ";
		}
		fout<<"0"<<endl;
	}
}
void DrugConnect::addEdgeExistenceClauses(){
	//max 10 digit variable allowed
	unsigned int edge,comp;
	for(edge=0; edge<this->m;edge++){
		for(comp=0;comp<this->k;comp++){
			fout<<EnCtoVr(edge,comp)<<" ";
		}
		fout<<"0"<<endl;
	}
}

void DrugConnect::addCliqueExistenceClauses(){
	unsigned int comp,edge;
	//each clique must have atleast one edge
	for(comp=0;comp<this->k;comp++){
		for(edge=0;edge<this->m;edge++){
			fout<<EnCtoVr(edge,comp)<<" ";
		}
		fout<<"0"<<endl;
	}
}

void DrugConnect::addVertexPairExistenceClauses(){
	unsigned int i,j,l,comp,len;
	for(i=0 ;i<this->n; i++){
		for(j=i+1;j<this->n;j++){
			if(adjacency[i][j]==-1){
				//~Edge -> ~(V1.V2)
				for(comp=0;comp<this->k;comp++){
					fout<<"-"<<VnCtoVr(i,comp)<<" ";
					fout<<"-"<<VnCtoVr(j,comp)<<" 0"<<endl;
				}
			}
		}
	}
}

void DrugConnect::addEdgeToVertexClauses(){
	//E<->V1.V2
	unsigned int comp,edge;
	int x,y,z;
	for(comp=0;comp<this->k;comp++){
		for(edge=0;edge<this->m;edge++){
			x=EnCtoVr(edge,comp);
			y=VnCtoVr((this->edges)[edge].first,comp);
			z=VnCtoVr((this->edges)[edge].second,comp);

			//write (-x+y).(-x+z).(x+-y+-z)
			printEqCluases(x,y,z,1);
		}
	}
}

void DrugConnect::printEqCluases(int a, int b, int c, int type){

    // type 1 == equivalance of a , b ^ c
    // type 2 == equivalence of a , b => c

    if(type == 1){
        fout << "-" << a << " " << b << " 0" <<endl;
        fout << "-" << a << " " << c << " 0" <<endl;
        fout << "-" << b << " -" << c << " " << a <<" 0" <<endl;
    }
    else if(type == 2){
        fout << "-" << c << " " << a << " 0" <<endl;
        fout << b << " " << a << " 0" <<endl;
        fout << "-" << a << " -" << b << " " << c <<" 0" <<endl;

    }
    else
        cout << "type not yet defined!\n";
}

void  DrugConnect::addMutualExclusionConstraint(int& varCount){

	int compVariables1[this->n];
	int compVariables2[this->n];
	unsigned int i,v,j;
	for(i = 0; i < this->k; i++){
		for(v = 0; v < this->n; v++)
			compVariables1[v] = VnCtoVr(v,i);

		for(j = i+1; j < this->k; j++){

			for(int v = 0; v < this->n; v++)
				compVariables2[v] = VnCtoVr(v,j);

			string str = "";
			for(int l = 0; l < n; l++){

				printEqCluases(varCount, compVariables1[l], compVariables2[l], 2);
				str.push_back('-');
				str+= Utility::intToString(varCount);
				str.push_back(' ');
				varCount++;
			}

			fout << str << "0" << endl;

			str = "";
			for(int l = 0; l < n; l++){

				printEqCluases(varCount, compVariables2[l], compVariables1[l], 2);
				str.push_back('-');
				str += Utility::intToString(varCount);
				str.push_back(' ');
				varCount++;
			}
			fout << str << "0" << endl;
		}
	}
}

int main(int argc, char* argv[]){
	//Getting file names
	if(!argv[1]){
		cout<<"No file Given.";
		return -1;
	}

	int len = strlen(argv[1]);
	char inFilePath[len+10];
	char outFilePath[len+10];
	strcpy(inFilePath,argv[1]);
	strcat(inFilePath,".graph");
	strcpy(outFilePath,argv[1]);
	strcat(outFilePath,".satinput");

	ifstream fin(inFilePath);
	if(!fin){
		cout<<"Error opening File. Exiting.";
		return 1;
	}
	int n,m,k,x,y;
	fin>>n>>m>>k;

	//Assuming no repeated edges are there
	iPair* edges = new iPair[m];
	for(int i=0;i<m;i++){
		fin>>x;
		fin>>y;
		edges[i] = make_pair(x-1,y-1);
	}
	//close the file
	fin.close();

	DrugConnect agencies(n,edges,m,k,outFilePath);

	agencies.addVertexExistenceClauses();
	agencies.addEdgeExistenceClauses();
	agencies.addCliqueExistenceClauses();
	agencies.addEdgeToVertexClauses();
	agencies.addVertexPairExistenceClauses();

	int varCount = (n+m)*k +1;
	agencies.addMutualExclusionConstraint(varCount);

	edges= NULL;
	return 0;
}
