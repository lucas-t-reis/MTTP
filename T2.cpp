#include <bits/stdc++.h>
using namespace std;

struct City{
	City(){}
	City(long double x_, long double y_, const vector<int>&items_, int remainingItems_) : x(x_), y(y_), items(items_), 
	remainingItems(remainingItems_){}
	long double x, y;
	vector <int> items;
	int remainingItems;
};

struct Node{
	Node(): id(0), capacity(0){}
	Node(int id_, const vector<int> &items_, int capacity_) : id(id_), items(items_), capacity(capacity_){}
	int id;
	vector <int> items;
	int capacity;
};

struct Item{
	Item(){}
	Item(int profit_, int weight_, int thief_, int city_) : profit(profit_), weight(weight_), thief(thief_),
	city(city_){}
	int profit;
	int weight;
	int thief;
	int city;
};

struct Thief{
	Thief():capacity(0), route(vector<Node>(1, Node())){}
	Thief(const vector<Node> &route_, int capacity_) : route(route_), capacity(capacity_) {}
	vector <Node> route;
	int capacity;
};

vector <City> cities;
vector <Item> items;
vector <Thief> gang;
vector <vector <int>> adj;
int gangCapacity = 0;


string name, type;
int V, M, W;
long double vMin, vMax, R;


int dist(int a, int b){
	return ceil(sqrt((cities[a].x - cities[b].x) * (cities[a].x - cities[b].x) + (cities[a].y - cities[b].y) * (cities[a].y - cities[b].y))); 
}

void readNum(int &a, long double &b, bool isInteger){

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

void readInstance(string &name, string &type, int &V, int &M, int &W, long double &vMin, long double &vMax, long double &R){
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
	long double tempDouble[3] = {0.0};

	while(k<3) readNum(temp[k++],tempDouble[0],true);
	
	V = temp[0];
	M = temp[1];
	W = temp[2];

	k=0;
	while(k<3) readNum(temp[0],tempDouble[k++], false);
	
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
		long double x, y;

		cin >> id >> x >> y;
	
		cities[i] = {x, y, vector<int>(), 0};
	}

	adj.resize(V);
	for(int i = 0; i < V; i++){
		adj[i].resize(V);
		for(int j = 0; j < V; j++){
			adj[i][j] = dist(i, j);
		}
	}

	// Ignoring last line from header
	getline(cin, line);

	// Copying items
	getline(cin, line);
	
	items.resize(M);
	
	for(int i = 0; i < M; i++){
		int id, p, w, idCity;

		cin >> id >> p >> w >> idCity;
	
		items[i] = {p, w, -1, idCity - 1};
		cities[idCity - 1].items.push_back(i);
		cities[idCity - 1].remainingItems++;
	}
}

int closestCity(int thiefId, int cityId){
	int best = adj[gang[thiefId].route[0].id][cityId];
	int bestI = 0;
	for(int i = 1; i < gang[thiefId].route.size(); i++){
		int from = gang[thiefId].route[i].id;
		if(best > adj[from][cityId]){
			best = adj[from][cityId];
			bestI = i;
		}
	}

	return bestI;
}

/*Vizinhanças*/

pair<int, int> opt2(int id){

	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	
	swap(gang[id].route[a], gang[id].route[b]);
	
	return make_pair(a, b);
}


pair<pair<int, int>, pair<int , int>> opt4(int id){
	
	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	int c = rand() % (gang[id].route.size() - 1) + 1;
	int d = rand() % (gang[id].route.size() - 1) + 1;
	
	swap(gang[id].route[a], gang[id].route[b]);
	swap(gang[id].route[c], gang[id].route[d]);
	
	auto p1 = make_pair(a, b);
	auto p2 = make_pair(c, d);

	return make_pair(p1,p2);
}

pair<int, Node> removeCity(int id){
	int cityId = rand() % (gang[id].route.size() - 1) + 1;

	auto p = make_pair(cityId, gang[id].route[cityId]);
	
	gang[id].route.erase(gang[id].route.begin() + cityId);
	
	return p;
}

pair<int, int> removeItem(int id){
	int cityId = rand() % (gang[id].route.size() - 1) + 1;
	int itemId = rand() % gang[id].route[cityId].items.size();

	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemId);
	gang[id].route[cityId].capacity -= items[itemId].weight;
	items[itemId].thief = -1;

	return make_pair(cityId, itemId);
}

pair<int, int> addItem(int id){
	int cont = 0;
	int itemId;
	do{
		itemId = rand() % items.size();
	}while((items[itemId].thief != -1 || items[itemId].weight + gangCapacity > W)  && cont++ < 100);

	if(cont >= 100){
		itemId = -1;
		for(int i = 0; i < items.size(); i++){
			if(items[i].thief == -1){
				itemId = i;
				break;
			}
		}
		if(itemId == -1) return make_pair(-1, -1);
	}

	gangCapacity += items[itemId].weight;
	items[itemId].thief = id;

	for(int i = 0; i < gang[id].route.size(); i++){
		if(items[itemId].city == gang[id].route[i].id){
			gang[id].route[i].items.push_back(itemId);
			gang[id].route[i].capacity += items[itemId].weight;
			return make_pair(itemId, -1 * items[itemId].city);
		}
	}

	int bestPlace = closestCity(id, items[itemId].city);
	if(bestPlace)
		gang[id].route.insert(gang[id].route.begin() + bestPlace, Node(items[itemId].city, {itemId}, items[itemId].weight));
	else
		gang[id].route.push_back(Node(items[itemId].city, {itemId}, items[itemId].weight));

	return make_pair(itemId, bestPlace);
}

/**/

long double cost(long double vMax, long double vMin, int W, long double R){


	long double total = 0.0;
	// Start from city 1 because a true thief never steals from home
	for(int i = 0; i < items.size(); i++){
		if(items[i].thief != -1){ 
			total += items[i].profit;
			//cerr << i << " ";
		}
	}
	// cerr << endl;

	// cerr << "Profit: " << total << endl;

	long double currPenalty = 0.0;
	long double v = (vMax - vMin)/W; // Defined in the problem description
	
	for(int i=0; i < gang.size(); i++){

		int capacity = 0;

		auto first = gang[i].route.front();
		auto end = gang[i].route.back();

		for(int j = 0; j < gang[i].route.size() - 1; j++){
			
			capacity += gang[i].route[j].capacity;

			int a = gang[i].route[j].id;
			int b = gang[i].route[j + 1].id;
			
			//cerr << adj[a][b]/(vMax - v * capacity) << endl;
			currPenalty += adj[a][b]/(vMax - v * capacity);
			cerr << i << " " << adj[a][b]/(vMax - v * capacity) << " " << (vMax - v * capacity) << endl;
		}

		// Coming back home
		capacity += end.capacity;
		currPenalty += adj[end.id][first.id]/(vMax - v * capacity);
		cerr << i << " " << adj[end.id][first.id]/(vMax - v * capacity) << " " << (vMax - v * capacity) << endl;
		cerr << endl;
		//cerr << adj[end.id][first.id]/(vMax - v * capacity) << endl;
	}

	total -= R * currPenalty;

	//cerr << "Penalty: " << R * currPenalty << endl;
	
	return total;
}

void greedyInitialSolution(int numThiefs){

	vector<int>actualPos(numThiefs, 0);
	vector<bool>ended(numThiefs, false);

	long double v = (vMax - vMin)/W;

	bool hasOption = true;
	while(hasOption){
		hasOption = false;
		for(int i = 0; i < numThiefs; i++){
			if(ended[i])
				continue;

			long double bestValue = 0.0;
			int bestItem = -1;

			for(int j = 0; j < M; j++){
				if(items[j].weight + gangCapacity > W || items[j].thief != -1)
					continue;
				
				long double goingCost = adj[actualPos[i]][items[j].city]/(vMax - v * gang[i].capacity);
				long double returnCost = adj[items[j].city][0]/(vMax - v * (gang[i].capacity + items[j].weight));
				
				//cout << actualPos[i] << " " << items[j].profit - goingCost - returnCost << " " << bestValue << " " << i <<" " << j << " " << items[j].city << endl;
				
				if(items[j].profit - goingCost - returnCost > bestValue){
					bestValue = items[j].profit - goingCost - returnCost;
					bestItem = j;
					hasOption = true;
				}
			}

			//cout << endl;

			if(bestItem == -1){
				actualPos[i] = 0;
				ended[0] = true;
			}
			else{
				if(gang[i].route.empty() || gang[i].route.back().id != items[bestItem].city){
					gang[i].route.push_back(Node());
					gang[i].route.back().id = items[bestItem].city;
					gang[i].route.back().capacity = 0;
				}
				gang[i].route.back().items.push_back(bestItem);
				gang[i].route.back().capacity += items[bestItem].weight;
				gang[i].capacity += items[bestItem].weight;
				gangCapacity += items[bestItem].weight;
				items[bestItem].thief = i;
				actualPos[i] = items[bestItem].city;
				//cerr << cost(vMax, vMin, W, R) << endl << endl;
			}
		}
	}
}

void joinNodes(int index, int i, int j){
	for(int k:gang[index].route[i].items){
		gang[index].route[j].items.push_back(k);
	}
	gang[index].route[j].capacity += gang[index].route[i].capacity;
	gang[index].route.erase(gang[index].route.begin() + i);
}

void fixRoute(int index){
	for(int i = 0; i < gang[index].route.size(); i++){
		for(int j = i + 1; j < gang[index].route.size(); j++){
			if(gang[index].route[i].id == gang[index].route[j].id ){
				cerr << "Fixed: " << i << " " << j << " " << gang[index].route[j].id << endl;
				joinNodes(index, i, j);
				i--;
				j--;
			}
		}
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));

	readInstance(name, type, V, M, W, vMin, vMax, R);

	int numThiefs = atoi(argv[1]);
	gang.resize(numThiefs);

	greedyInitialSolution(numThiefs);
	for(int i = 0; i < numThiefs; i++)
		fixRoute(i);

	// for(int i = 0; i < numThiefs; i++){
	// 	for(auto j:gang[i].route){
	// 		cout << j.id << " ";
	// 	}
	// 	cout << endl;
	// }

	for(int i = 0; i < gang.size(); i++){
		vector<int>I;
		int cap = 0;
		for(int j = 1; j < gang[i].route.size(); j++){
			if(j != gang[i].route.size() - 1)
				cout << gang[i].route[j].id + 1 << ",";
			else
				cout << gang[i].route[j].id + 1;
			for(int k = 0; k < gang[i].route[j].items.size(); k++){
				I.push_back(gang[i].route[j].items[k]);
				//s.insert(gang[i].route[j].items[k]);
			}
		}
		bool virg = false;
		for(auto j:I){
			if(!virg){
				cout << j + 1;
				virg = true;
			}
			else{
				cout << "," << j + 1;
			}
		}
		cout << endl;
	}

	// for(auto i:s){
	// 	cerr << i << " ";
	// }
	// cerr << endl;

	cerr << cost(vMax, vMin, W, R) << endl;
	cerr << gangCapacity << " " << W << endl;

	// gang.push_back(Thief(vector<Node>(4), 12));
	// gang[0].route[0] = Node(0, vector<int>(), 0);
	// gang[0].route[1] = Node(1, {0, 1}, 5);
	// gang[0].route[2] = Node(2, {2, 3, 4}, 5);
	// //gang[0].route[3] = Node(3, {5}, 2);

	// for(int i = 0; i < items.size(); i++) items[i].thief = 0;
	// items[5].thief = -1;
	// gangCapacity = 10;

	// auto p =addItem(0);
	// cout << p.first << " " << p.second << endl;
	// cout << cost(vMax, vMin, W, R) << endl;

}
