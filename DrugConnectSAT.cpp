#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

class DrugConnect{
public:
	int n; //vertices count
	int m; //edges count
	int k; //cliques
	int** adjacency;
	vector<char*> cnf;

	void addVertexExistenceClauses();
	char* intToString(int);
	int VnCtoVr(int vertex,int component);
	int EnCtoVr(pair<int,int> edge,int component);
	pair<pair<int,int>,int> VrtoEnC(int);
	pair<int,int> VrtoVnC(int);
	DrugConnect(int n,pair<int,int>* edges,int m,int k){
		this->n = n;
		this->m = m;
		this->k =k;

		int i,j;
		adjacency = new int* [this->n];
		for(i=0;i<this->n;i++){
			adjacency[i] = new int[this->n];
		}
		for(i=0;i<this->n;i++){
			for(j=0;j<this->n;j++){
				adjacency[i][j]=0;
			}
		}

		pair<int,int> edge;
		for(i=0;i<this->m;i++){
			edge = edges[i];
			adjacency[edge.first][edge.second] = 1;
			adjacency[edge.second][edge.first] = 1;
		}
	}
	~DrugConnect(){
		unsigned int i;
		for(i=0;i< this->n;i++){
			delete[] adjacency[i];
			adjacency[i] = NULL;
		}
		delete[] adjacency;
		adjacency = NULL;
	}
	
};

int main(){
	int n,m,k;
	cin>>n>>m>>k;

	pair<int,int> edges[m];
	for(int i=0;i<m;i++){
		cin>>edges[i].first;
		cin>>edges[i].second;
	}

	DrugConnect agencies(n,edges,m,k);

}

char* DrugConnect::intToString(int x){
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
void DrugConnect::addVertexExistenceClauses(){
	char clause[11*this->k +1]; //max 10 digit variable allowed
	int vr;
	for(int vertex=0; vertex<this->n;vertex++){
		strcpy(clause,"");
		for(int j=0;j<this->k;j++){
			vr = VnCtoVr(vertex,j);
			strcat(clause,intToString(vr));
			strcat(clause," ");
		}
		//clause will contain one extra space
		cnf.push_back(clause);
	}
}
