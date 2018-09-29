#include <iostream>
#include <cstring>
#include <vector>
#include <tr1/unordered_map>
using namespace std;

template <class T>
struct comparator
{
    bool operator()(const T& x, const T& y) const
    { return strcmp(x, y) == 0; }
};
struct Hash_Code{
    //BKDR hash algorithm
    int operator()(char * str)const
    {
        int seed = 131;
        int hash = 0;
        while(*str)
        {
            hash = (hash * seed) + (*str);
            str ++;
        }

        return hash & (0x7FFFFFFF);
    }
};
typedef tr1::unordered_map<char*, unsigned int, Hash_Code ,comparator<char*> > unorderedMap;
typedef pair<int,int> iPair;


class DrugConnect{
public:
	int n; //vertices count
	int m; //edges count
	int k; //cliques
	iPair* edges;
	unorderedMap edgeMap;
	vector<char*> cnf;

	void addVertexExistenceClauses();
	void addEdgeExistenceClauses();
	char* intToString(int);
	char* edgeToString(iPair edge);
	int VnCtoVr(int vertex,int component);
	int EnCtoVr(iPair edge,int component);
	pair<iPair,int> VrtoEnC(int);
	iPair VrtoVnC(int);
	DrugConnect(int n,iPair* edges,int m,int k){
		this->n = n;
		this->m = m;
		this->k =k;
		this->edges =edges;
		//initialize edgeMap
		for(int i=0;i<m;i++){
			edgeMap.insert(make_pair(edgeToString(edges[i]), i));
		}
	}
	~DrugConnect(){
		//Free map memory
		unorderedMap:: iterator itr;
		for (itr = edgeMap.begin(); itr != edgeMap.end(); itr++){
			delete[] itr->first;
			//tried making element NULL but failed..
		}
		//Delete Edges
		delete[] edges;
		edges = NULL;
	}
};

int main(){
	int n,m,k;
	cin>>n>>m>>k;

	iPair* edges = new iPair[m];
	for(int i=0;i<m;i++){
		cin>>edges[i].first;
		cin>>edges[i].second;
	}

	DrugConnect agencies(n,edges,m,k);


	edges = NULL;
	return 0;
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

char* DrugConnect::edgeToString(iPair edge){

	char* edgeStr = new char[22]; //(u,v) can be max of 22 size
	char* val = intToString(edge.first);
	strcpy(edgeStr,val);
	strcat(edgeStr,",");

	//Free Memory
	delete[] val;

	val= intToString(edge.second);
	strcat(edgeStr,val);

	//Free Memory
	delete[] val;

	return edgeStr;
}
void DrugConnect::addVertexExistenceClauses(){
	//max 10 digit variable allowed
	char* var = NULL;
	char* clause = NULL;
	for(int vertex=0; vertex<this->n;vertex++){
		clause = new char[11*this->k +1];
		for(int j=0;j<this->k;j++){
			var = intToString(VnCtoVr(vertex,j));
			strcpy(clause,var);
			strcat(clause," ");

			//Free edgeVar
			delete[] var;
			var = NULL;
		}
		//clause will contain one extra space
		cnf.push_back(clause);
	}
}

void DrugConnect::addEdgeExistenceClauses(){
	char* edgeVar = NULL; //it is a variable
	iPair edge;
	char* clause = NULL;
	for(int i=0;i<m;i++){
		edge = (this->edges)[i];
		clause = new char[11*this->k +1];//max 10 digit variable allowed
		for(int j=0;j<k;j++){
			edgeVar = intToString(EnCtoVr(edge,k));
			strcpy(clause,edgeVar);
			strcat(clause," ");

			//Free edgeVar
			delete[] edgeVar;
			edgeVar = NULL;
		}
		//clause will contain one extra space
		cnf.push_back(clause);
	}
}

