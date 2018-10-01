#include "CnfConversion.cpp"
#include <tr1/unordered_map>
#include <fstream>
#include <cmath>

typedef pair<int,int> iPair;


class DrugConnect{
public:
	unsigned int n; //vertices count
	unsigned int m; //edges count
	unsigned int k; //cliques
	bool** adjacency;
	ofstream fout;

	int VnCtoVr(int vertex,int component);
	void addVertexExistenceClauses();
	void addCliqueConstraint();
	void addMutualExclusionConstraint();
	void addNotEmptyCliqueRestriction();
	void subset(int a[], int size, int num);

	DrugConnect(unsigned int n,iPair* edges,unsigned int m,unsigned int k,char* filePath){
		this->n = n;
		this->m = m;
		this->k =k;

		unsigned int i;
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

		fout.open(filePath, ios::out);
		fout<<"p cnf "<<this->n<<endl;
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

		fout.close();

	}
};

int DrugConnect::VnCtoVr(int vertex,int component){
	return k * vertex + component + 1;
}

void DrugConnect::addVertexExistenceClauses(){
	//max 10 digit variable allowed
	unsigned int vertex,j;
	for(vertex=0; vertex<this->n;vertex++){
		for(j=0;j<this->k;j++){
			fout<<VnCtoVr(vertex,j)<<" ";
		}
		fout<<"0"<<endl;
	}
}

void DrugConnect::addNotEmptyCliqueRestriction(){
	unsigned int i,j,l,comp,len;
	char* v1 =NULL;
	char* v2 = NULL;
	char clause[22*this->m+1];
	//each clique must have atleast one edge
	for(comp=0;comp<this->k;comp++){
		strcpy(clause,"");
		for(i=0 ;i<this->n; i++){
			for(j=i+1;j< this->n;j++){
				if(adjacency[i][j] == true){
					v1 = Expr::intToString(VnCtoVr(i,comp));
					v2 = Expr::intToString(VnCtoVr(j,comp));
					//add cnf form....
					strcat(clause, v1);
					strcat(clause, ".");
					strcat(clause, v2);
					strcat(clause, "+");
					//Free Memory
					delete[] v1;
					delete[] v2;
					v1= NULL;
					v2 = NULL;
				}
			}
		}
		//remove extra + from the end;
		len = strlen(clause);
		clause[len-1] = '\0';
		//convert clause to cnf form
		vector<char*> clauses;
		ConvertingRules::getCNF(clause,clauses);
		for(l=0;l<clauses.size();l++){
			fout.write(clauses[l], strlen(clauses[l]));
			fout<<" 0"<<endl;
		}
		//Free memory
		for(l=0;l<clauses.size();l++){
			delete[] clauses[l];
			clauses[l]=NULL;
		}
	}
}
void DrugConnect::addCliqueConstraint(){
	unsigned int i,j,l,comp,len;
	char* v1 =NULL;
	char* v2 = NULL;
	char clause[22*k+1];
	for(i=0 ;i<this->n; i++){
		for(j=i+1;j< this->n;j++){
			if(adjacency[i][j] == true){
				//this edge must be present in atleast one clique
				strcpy(clause,"");
				for(comp=0;comp<this->k;comp++){
					v1 = Expr::intToString(VnCtoVr(i,comp));
					v2 = Expr::intToString(VnCtoVr(j,comp));
					//add cnf form....
					strcat(clause, v1);
					strcat(clause, ".");
					strcat(clause, v2);
					strcat(clause, "+");

					//Free Memory
					delete[] v1;
					delete[] v2;
					v1= NULL;
					v2 = NULL;
				}
				//remove extra + from the end;
				len = strlen(clause);
				clause[len-1] = '\0';
				//convert clause to cnf form
				vector<char*> clauses;
				ConvertingRules::getCNF(clause,clauses);
				for(l=0;l<clauses.size();l++){
					fout.write(clauses[l], strlen(clauses[l]));
					fout<<" 0"<<endl;
				}
				//Free memory
				for(l=0;l<clauses.size();l++){
					delete[] clauses[l];
					clauses[l]=NULL;
				}
			}
			else{
				//~Edge -> ~(V1.V2)
				for(comp=0;comp<this->k;comp++){
					fout<<"-"<<VnCtoVr(i,comp)<<" ";
					fout<<"-"<<VnCtoVr(j,comp)<<" 0"<<endl;
				}
			}
		}
	}
}

void DrugConnect::addMutualExclusionConstraint(){

	int compVariables1[n];
	int compVariables2[n];
	double po = pow(2, n);
	int subsetIndex[n];
	long long int count = 0;
	unsigned int i,j,comp;

	for(comp = 0; comp < k; comp++){

		for(i = 0; i < n; i++)
			compVariables1[i] = VnCtoVr(i, comp);

		cout << "***************************************COMPONENT NUMBER 1 = " << comp << endl;

		for(j = 0; j < k; j++){
			if(comp != j){
				for(i = 0; i < n; i++)
					compVariables2[i] = VnCtoVr(i, j);

				cout << "*******************COMPONENT NUMBER 2 = " << comp << endl;

				for(int i = 0; i < po; i++)
				{
					if(ceil(log2(po-i-1)) == floor(log2(po-i-1)))
						continue;
					subset(subsetIndex, n, i); // return a subset where the bits elements are 1.
					cout << "Subset with index i = " << i << endl;
					for(j = 0; j < n; j++){
						fout << compVariables1[j]*subsetIndex[j] << " ";
					}
					for(j = 0; j < n; j++){
						if(subsetIndex[j] == -1){
							fout << "-" << compVariables2[j] << " ";
						}
					}
					fout <<"0"<<endl;
					count++;
					if(count % 1000000 == 0 )
						cout << count << endl;
				}

			}
		}
	}
	cout << "total variables = " << k*n << "\nTotal clauses printed = " << count<< endl;
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
	iPair edges[m];
	for(int i=0;i<m;i++){
		fin>>x;
		fin>>y;
		edges[i] = make_pair(x-1,y-1);
	}
	//close the file
	fin.close();

	DrugConnect agencies(n,edges,m,k,outFilePath);
	agencies.addVertexExistenceClauses();
	agencies.addNotEmptyCliqueRestriction();
	agencies.addCliqueConstraint();
	agencies.addMutualExclusionConstraint();
	return 0;
}
