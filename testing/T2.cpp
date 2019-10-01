#include <bits/stdc++.h>
using namespace std;

struct City{
	double x, y;
	vector <int> items;
};

struct Node{
	int id;
	list <int> items;
	int capacity;
};

struct Item{
	int profit;
	int weight;
	int thief;
};

struct Thief{
	list <Node> route;
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
	while(k<3) readNum(temp[0],tempDouble[k++], -1);
	
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

double cost(double vMax, double vMin, int W, double R){


	double total = 0.0;
	// Start from city 1 because a true thief never steals from home
	for(int j = 0; j < items.size(); j++)
		if(items[j].thief != -1) 
			total += items[j].profit;
	
	double currPenalty = 0.0;
	double v = (vMax - vMin)/W; // Defined in the problem description
	for(int i=0; i < gang.size(); i++){

		int capacity = 0;

		auto first = gang[i].route.front();
		auto end = gang[i].route.back();

		for(auto j = gang[i].route.begin(); next(j) != gang[i].route.end(); j++){
			
			capacity += j->capacity;

			int a = j->id;
			int b = (next(j))->id;
			
			currPenalty += adj[a][b]/(vMax - v * capacity);
		}

		capacity += end.capacity;
		currPenalty += adj[end.id][first.id]/(vMax - v * capacity);

		// Coming back home
		currPenalty += adj[cities.size()-2][cities.size()-1]/(vMax - v*gang[i].capacity); 
	}

	total -= R * currPenalty;

	return total;
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
