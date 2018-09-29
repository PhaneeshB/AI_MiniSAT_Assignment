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
		for(unsigned int i=0;i<m;i++){
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

int DrugConnect::VnCtoVr(int vertex,int component){
	return k * vertex + component + 1;
}

pair<int,int> DrugConnect::VrtoVnC(int variable){
	
	variable--;
	int vertex = variable / k;
	int component = variable % k;
	return make_pair(vertex, component);
}

int DrugConnect::EnCtoVr(pair<int, int> edge, int component){
	
	char* edgeString = edgeToString(edge);
	unsigned int edgeNum = edgeMap.find(edgeString);
	return edgeNum * k + component + 1;
}

pair<pair<int,int>,int> DrugConnect::VrtoEnC(int variable){

	int offset = k * n;
	variable -= offset + 1;
	int edgeNum  = variable / k;
	int component = variable % k;
	return make_pair(edges[edgeNum], component);
}

int main1(){
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

int main(){
	
	int n,m,k;
	cin>>n>>m>>k;

	iPair* edges = new iPair[m];
	for(int i=0;i<m;i++){
		cin>>edges[i].first;
		cin>>edges[i].second;
	}

	DrugConnect agencies(n,edges,m,k);

	cout << "Number of variables = " << k*(n+m) << endl;
	cout << "Map: graph to Variable:\n";
	int v;
	for(int i = 0; i <n; i++){
		for(int j = 0; j< k; j++ ){
			v = agencies.VnCtoVr(i , j);
			cout << v <<" "; 
		}
		cout << endl;
	}

	cout << "VR to VnC\n";
	for(int i = 0; i <m; i++){
		for(int j = 0; j< k; j++ ){
			v = agencies.EnCtoVr(edges[i] , j);
			cout << v <<" "; 
		}
		cout << endl;
	}

	cout<<"Variable to Vertex \n";
	for(int i = 1; i <= k*n; i++){
 		pair<int,int> x = agencies.VrtoVnC(i); 
		cout << i << " = " << x.first << "	"<< x.second << endl;
	}

	cout << "vari to Edge and component" << endl;
	for(int i = k*n +1; i <= k*(n+m); i++){
 		pair<pair<int,int>, int> x = agencies.VrtoEnC(i); 
		cout << i << " = " << x.first.first << "	" << x.first.second << "	"<< x.second << endl;
	}

	// Test Code
	// int adjacency[n][n];
	// pair<int,int> edge;
	// for(int i = 0; i < m; i++ ){
	// 	edge = edges[i];
	// 	adjacency[edge.first][edge.second] = 1;
	// 	adjacency[edge.second][edge.first] = 1;
	// }

	// ofstream out;
	// out.open("test1.satinput", ios::app);
	// //V^V => E
	// for(int i =0; i < k; i++){

	// 	for(int j =0 ; j < n; j++){

	// 		for(int l = j+1; l < n; l++){

	// 			out << "-" << VnCtoVr(j, i) << " " << "-" << VnCtoVr(l, i);
	// 			if(adjacency[j][l] == 1)
	// 				out << " " <<EnCtoVr(make_pair(j, l), i);
	// 			out << endl;
	// 		}
	// 	}
	// }
	// //V ^ ~V => ~E
	// for(int i =0; i < k; i++){

	// 	for(int j =0 ; j < n; j++){

	// 		for(int l = j+1; l < n; l++){

	// 			out << "-" << VnCtoVr(j, i) << " " << VnCtoVr(l, i);
	// 			if(adjacency[j][l] == 1)
	// 				out << " -" <<EnCtoVr(make_pair(j, l), i);
	// 			out << endl;
	// 		}
	// 	}
	// }

	edges = NULL;
	//out.close();
	return 0;
}