#include <bits/stdc++.h>
using namespace std;

struct City{
	double x, y;
	vector <int> items;
};

struct Item{
	int profit;
	int weight;
	bool taken;
};

struct Thief{
	vector <int> route;
	vector <int> bag;
	int capacity;
	double speed;
};

vector <City> cities;
vector <Item> items;
vector <Thief> gang;
vector <vector <double>> adj;


double dist(int a, int b){
	return sqrt((cities[a].x - cities[b].x) * (cities[a].x - cities[b].x) + (cities[a].y - cities[b].y) * (cities[a].y - cities[b].y)); 
}

void readNum(int &a, double &b, bool isInteger){

	stringstream ss;
	string line;
	getline(cin, line);
	ss << line;

	while(ss>>line) {

		if(line[line.size()-1]==':'){

			ss >> line;
			stringstream ss2(line);	
			
			if(isInteger) ss2 >> a;
			else ss2 >> b;
			break;
		
		}

	}
}

void readInstance(string &name, string &type, int &V, int &M, int &W, double &vMin, double &vMax, double &R){
	// Reading name and type	
	stringstream ss;
	string line;
	getline(cin, line);
	ss << line;

	while(ss>>line) {
		if(line[line.size()-1]==':'){
			ss >> name;
			break;
		}
	}

	getline(cin,line);
	ss.clear();
	ss << line;
	
	while(ss>>line) {
		if(line[line.size()-1]==':'){
			getline(ss, type);
			break;
		}
	}

	
	// Getting the numeric part of the data
	int k=0;
	int temp[3] = {0};
	double tempDouble[3] = {0.0};

	while(k<3) readNum(temp[k++],tempDouble[0],true);
	
	V = temp[0];
	M = temp[1];
	W = temp[2];

	k=0;
	while(k<3) readNum(temp[0],tempDouble[k++],false);
	
	vMin = tempDouble[0];
	vMax = tempDouble[1];
	R = tempDouble[2];


	// Ignoring last line from header
	getline(cin, line);


	// Copying city coordinates
	getline(cin, line);
	
	cities.resize(V);
	
	for(int i = 0; i < V; i++){
		int id;
		double x, y;

		cin >> id >> x >> y;
	
		cities[i] = {x, y, vector<int>()};
	}

	adj.resize(V);
	for(int i = 0; i < V; i++){
		adj[i].resize(V);
		for(int j = 0; j < V; j++){
			adj[i][j] = dist(i, j);
		}
	}

	cin.ignore();

	// Copying items
	getline(cin, line);
	
	items.resize(M);
	
	for(int i = 0; i < M; i++){
		int id, p, w, idCity;

		cin >> id >> p >> w >> idCity;
	
		items[i] = {p, w, false};
		cities[idCity - 1].items.push_back(i);
	}
}

int main() {

	string name, type;
	int V, M, W;
	double vMin, vMax, R;
	readInstance(name, type, V, M, W, vMin, vMax, R);
	
	// Conferindo entrada...
	cout << name << endl;
	cout << type << endl;
	cout << V << endl;
	cout << M << endl;
	cout << W << endl;
	cout << vMin << endl;
	cout << vMax << endl;
	cout << R << endl;
	
	cout << "Cities" << endl;

	int cont = 0;

	for(int i = 0; i < V; i++){
		for(int j = 0; j < V; j++){
			cout << adj[i][j] << " ";
		}
		cout << "\n";
	}
}
