#include <iostream>

using namespace std;

class DrugConnect{
public:
	int n; //vertices count
	int m; //edges count
	int k; //cliques
	int** adjacency;
	DrugConnect(int n,pair<int,int>* edges,int m,int k){
		this->n = n;
		this->m = m;
		this->k =k;

		int i,j;
		adjacency = new int* [n];
		for(i=0;i<n;i++){
			adjacency[i] = new int[n];
		}
		for(i=0;i<n;i++){
			for(j=0;j<n;j++){
				adjacency[i][j]=0;
			}
		}

		pair<int,int> edge;
		for(i=0;i<m;i++){
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

