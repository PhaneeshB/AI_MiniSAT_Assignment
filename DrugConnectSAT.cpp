#include "CnfConversion.cpp"
#include <tr1/unordered_map>
#include <fstream>
#include <cmath>

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
	unsigned int n; //vertices count
	unsigned int m; //edges count
	unsigned int k; //cliques
	iPair* edges;
	unorderedMap edgeMap;
	vector<char*> cnf;
	bool** adjacency;

	static char* edgeToString(iPair edge);
	int VnCtoVr(int vertex,int component);
	int EnCtoVr(iPair edge,int component);
	pair<iPair,int> VrtoEnC(int);
	iPair VrtoVnC(int);
	void addVertexExistenceClauses();
	void addEdgeExistenceClauses();
	void addCliqueConstraint();
	void addMutualExclusionConstraint();
	void subset(int a[], int size, int num);

	DrugConnect(unsigned int n,iPair* edges,unsigned int m,unsigned int k){
		this->n = n;
		this->m = m;
		this->k =k;
		this->edges =edges;

		//sort each edge(u,v) and initialize edgeMap
		int temp;
		unsigned int i;
		for(i=0;i<m;i++){
			if((this->edges)[i].first > (this->edges)[i].second){
				temp = (this->edges)[i].first;
				(this->edges)[i].first = (this->edges)[i].second;
				(this->edges)[i].second = temp;
			}
			edgeMap.insert(make_pair(edgeToString((this->edges)[i]), i));
		}

		//initialize adjacency matrix
		adjacency = new bool* [this->n];
		for(i=0;i<this->n;i++){
			adjacency[i] = new bool[this->n];
			memset(adjacency[i], false, this->n*sizeof(bool));
		}

		pair<int,int> edge;
		for(i=0;i<this->m;i++){
			edge = edges[i];
			adjacency[edge.first][edge.second] = true;
			adjacency[edge.second][edge.first] = true;

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
		if(edges!=NULL){
			delete[] edges;
			edges = NULL;
		}

		//Free cnf
		unsigned int i;
		for(i=0;i<cnf.size();i++){
			delete[] cnf[i];
			cnf[i] = NULL;
		}
		//Free Adjacency
		for(i=0;i<this->n;i++){
			delete[] adjacency[i];
			adjacency[i] = NULL;
		}
		delete[] adjacency;
		adjacency = NULL;

	}
};

char* DrugConnect::edgeToString(iPair edge){

	char* edgeStr = new char[22]; //(u,v) can be max of 22 size
	char* val = Expr::Expr::intToString(edge.first);
	strcpy(edgeStr,val);
	strcat(edgeStr,",");

	//Free Memory
	delete[] val;

	val= Expr::Expr::intToString(edge.second);
	strcat(edgeStr,val);

	//Free Memory
	delete[] val;

	return edgeStr;
}
int DrugConnect::VnCtoVr(int vertex,int component){
	return k * vertex + component + 1;
}

int DrugConnect::EnCtoVr(iPair edge, int component){

	char* edgeString = edgeToString(edge);
	unsigned int edgeNum = (edgeMap.find(edgeString))->second;
	//Free Memory
	delete[] edgeString;
	edgeString = NULL;

	return edgeNum * this->k + component + 1;
}

iPair DrugConnect::VrtoVnC(int variable){

	variable--;
	int vertex = variable / this->k;
	int component = variable % this->k;
	return make_pair(vertex, component);
}

pair<iPair,int> DrugConnect::VrtoEnC(int variable){

	int offset = this->k * this->n;
	variable -= offset + 1;
	int edgeNum  = variable / this->k;
	int component = variable % this->k;
	return make_pair((this->edges)[edgeNum], component);
}

void DrugConnect::addVertexExistenceClauses(){
	//max 10 digit variable allowed
	char* var = NULL;
	char* clause = NULL;
	unsigned int vertex,j;
	for(vertex=0; vertex<this->n;vertex++){
		clause = new char[11*this->k +1];
		strcpy(clause,"");
		for(j=0;j<this->k;j++){
			var = Expr::intToString(VnCtoVr(vertex,j));
			strcat(clause,var);
			if(j!=this->k-1)
				strcat(clause," ");

			//Free edgeVar
			delete[] var;
			var = NULL;
		}
		cnf.push_back(clause);
	}
}

void DrugConnect::addEdgeExistenceClauses(){
	char* edgeVar = NULL; //it is a variable
	iPair edge;
	char* clause = NULL;
	unsigned int i,j;
	for(i=0;i<this->m;i++){
		edge = (this->edges)[i];
		clause = new char[11*this->k +1];//max 10 digit variable allowed
		strcpy(clause, "");
		for(j=0;j<this->k;j++){
			edgeVar = Expr::intToString(EnCtoVr(edge,k));
			strcat(clause,edgeVar);
			if(j!=this->k-1)
				strcat(clause," ");

			//Free edgeVar
			delete[] edgeVar;
			edgeVar = NULL;
		}
		cnf.push_back(clause);
	}
}

void DrugConnect::addCliqueConstraint(){
	unsigned int i,j,l,comp;
	char* v1 =NULL;
	char* v2 = NULL;
	char* clause = NULL;
	for(i=0 ;i<this->n; i++){
		for(j=i+1;j< this->n;j++){
			if(adjacency[i][j] == true){
				//this edge must be present in atleast one clique
				clause = new char[22*k+1];
				strcpy(clause,"");
				for(comp=0;comp<this->k;comp++){
					v1 = Expr::intToString(VnCtoVr(i,comp));
					v2 = Expr::intToString(VnCtoVr(j,comp));
					//add cnf form....
					strcat(clause, v1);
					strcat(clause, ".");
					strcat(clause, v2);
					if(comp!=this->k-1)
						strcat(clause, "+");

					//Free Memory
					delete[] v1;
					delete[] v2;
					v1= NULL;
					v2 = NULL;
				}
				//convert clause to cnf form
				vector<char*> clauses;
				ConvertingRules::getCNF(clause,clauses);
				for(l=0;l<clauses.size();l++){
					cnf.push_back(clauses[l]);
				}

				//Free clause
				delete[] clause;
				clause = NULL;
			}
			else{
				//~Edge -> ~(V1.V2)
				for(comp=0;comp<this->k;comp++){
					clause = new char[25];
					v1 = Expr::intToString(VnCtoVr(i,comp));
					v2 = Expr::intToString(VnCtoVr(j,comp));
					strcpy(clause, "-");
					strcat(clause, v1);
					strcat(clause, " ");
					strcat(clause, "-");
					strcat(clause, v2);
					cnf.push_back(clause);
					//Free Memory
					delete[] v1;
					delete[] v2;
					v1= NULL;
					v2 = NULL;
				}
			}
		}
	}
}

void DrugConnect::addMutualExclusionConstraint(){

	int sub[n];
	int compVariables1[n];
	int compVariables2[n];
	double po = pow(2, n);
	int subsetIndex[n];
	long long int count = 0;
	ofstream out;
	out.open("test.satinput", ios::app);

	for(int comp = 0; comp < k; comp++){	
		
		for(int i = 0; i < n; i++)
			compVariables1[i] = VnCtoVr(i, comp);
		
		cout << "***************************************COMPONENT NUMBER 1 = " << comp << endl;

		for(int j = 0; j < k; j++){
			if(comp != j){
				for(int i = 0; i < n; i++)
					compVariables2[i] = VnCtoVr(i, j);
				
				cout << "*******************COMPONENT NUMBER 2 = " << comp << endl;
				
				for(int i = 0; i < po; i++)
				{
					if(ceil(log2(po-i-1)) == floor(log2(po-i-1)))
						continue;
					subset(subsetIndex, n, i); // return a subset where the bits elements are 1.
					cout << "Subset with index i = " << i << endl;
					for(int j = 0; j < n; j++){
						out << compVariables1[j]*subsetIndex[j] << " ";
					}
					for(int j = 0; j < n; j++){
						if(subsetIndex[j] == -1){
							out << "-" << compVariables2[j] << " ";
						}
					}
					out << endl;
					count++;
					if(count % 1000000 == 0 )
						cout << count << endl;
				}
				
			}
		}
	}
	cout << "total variables = " << k*n << "\nTotal clauses printed = " << count<< endl;
	out.close();
}

void DrugConnect::subset(int a[], int size, int num) {
    for(int k=0; k<size; k++){
        //if the k-th bit is set in num
        if( (1<<k) & num)
            a[size-k-1] = 1;
        else
        	a[size-k-1] = -1;
    }
}

int main(int argc, char* argv[]){

	ifstream fin("test.graph");
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

	DrugConnect agencies(n,edges,m,k);
	agencies.addVertexExistenceClauses();
	agencies.addCliqueConstraint();

	//Write cnf into a file
	ofstream fout("test.satinput");
	if(!fout){
		cout<<"Error opening File. Exiting.";
		return 1;
	}
	for(unsigned int i=0;i<(agencies.cnf).size();i++){
		fout<<(agencies.cnf)[i]<<endl;
	}
	//close file
	fout.close();

	agencies.addMutualExclusionConstraint();

	edges = NULL;
	return 0;
}


