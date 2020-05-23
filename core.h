#ifndef CORE_H
#define CORE_H

#include <vector>
#include <utility>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include <iomanip>

struct City {

	City(){}
	City(long double x_, long double y_) : x(x_), y(y_) {}

	long double x, y;

};

// Each node represents a city within the thiefs route
struct Node {
	
    Node(): id(0), capacity(0){}
	Node(int id_, const std::vector<int> &items_, int capacity_) : id(id_), items(items_), capacity(capacity_){}

	int id;                     // City Identification
	std::vector <int> items;    // Indices dos items que esse ladrao pegou nessa cidade
	int capacity;               // Total weight of items taken from this city

};

struct Item {

	Item(){}
	Item(int profit_, int weight_, int thief_, int city_) : profit(profit_), weight(weight_), thief(thief_), city(city_){}
	int profit; // Item value
	int weight; 
	int thief;  // Which thief currently has this item ( -1 is a flag for none )
	int city;   // From which city the item comes from 

};

struct Thief{
	
    Thief() : capacity(0), route(std::vector<Node>(1, Node())){}
	Thief(const std::vector<Node> &route_, int capacity_) : route(route_), capacity(capacity_) {}
	
    std::vector <Node> route;   // Self-explanatory. Beginning and end nodes do not repeat
	int capacity;               // Current weight being carried by this thief

};

// Problem variables
std::vector <City> cities;              
std::vector <Item> items; 
std::vector <Thief> gang; 
std::vector <std::vector <int> > adj;   // Cities adjacency matrix
int gangCapacity = 0;                   // Current global weight - sum of each Thiefs @capacity


std::string name, type;     // Instance name and type
int V, M, W;                // # of Cities and Items as well as max global weight
long double vMin, vMax, R;  // Minimum/Maximum velocity and Backpack fee

std::chrono::time_point<std::chrono::system_clock> start; // Metaheuristic timer


/* --------------------- AUXILIAR FUNCTIONS --------------------- */

bool timeLimit(long double maxSeconds) {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() > maxSeconds;
}

int dist(int a, int b) {
	return ceil(sqrt((cities[a].x - cities[b].x) * (cities[a].x - cities[b].x) + (cities[a].y - cities[b].y) * (cities[a].y - cities[b].y))); 
}

// Reads the numerical part from the Input
void readNum(int &a, long double &b, bool isInteger) {

	std::stringstream ss;
	std::string line;

	std::getline(std::cin, line);

	ss << line;
	while(ss>>line) {

		if(line[line.size()-1]==':'){

			ss >> line;
			std::stringstream ss2(line);	
			
			if(isInteger) ss2 >> a;
			else ss2 >> b;
			break;
		
		}

	}
}

void readInstance(std::string &name, std::string &type, int &V, int &M, int &W, long double &vMin, long double &vMax, long double &R) {
	
    // Instance type	
	std::stringstream ss;
	std::string line;

	std::getline(std::cin, line);

	ss << line;
	while(ss>>line) {
		if(line[line.size()-1]==':'){
			ss >> name;
			break;
		}
	}

	std::getline(std::cin,line);
	ss.clear();

	ss << line;
	while(ss>>line) {
		if(line[line.size()-1]==':'){
			std::getline(ss, type);
			break;
		}
	}

	// Reading numerical data
	int k=0;
	int temp[3] = {0};
	long double tempDouble[3] = {0.0};

	while(k<3) readNum(temp[k++],tempDouble[0],true);
	
	V = temp[0]; // # of cities
	M = temp[1]; // # of items
	W = temp[2]; // max weight

	k=0;
	while(k<3) readNum(temp[0],tempDouble[k++], false);
	
	vMin = tempDouble[0]; 
	vMax = tempDouble[1]; 
	R = tempDouble[2];      // Backpack fee

	// Ignoring last line from header
	std::getline(std::cin, line);

	// Copying city coordinates
	std::getline(std::cin, line);
	
	cities.resize(V);

	// Reading city coordinates
	for(int i = 0; i < V; i++){
		int id;
		long double x, y;

		std::cin >> id >> x >> y;
	
		cities[i] = {x, y};
	}

	// Building cities adjacency matrix
	adj.resize(V);
	for(int i = 0; i < V; i++){
		adj[i].resize(V);
		for(int j = 0; j < V; j++){
			adj[i][j] = dist(i, j);
		}
	}

	// Ignoring last line from header
	std::getline(std::cin, line);

	// Copying items
	std::getline(std::cin, line);
	
	items.resize(M);
	
	// Reading items
	for(int i = 0; i < M; i++){
		int id, p, w, idCity; 

		std::cin >> id >> p >> w >> idCity;
	
		items[i] = {p, w, -1, idCity - 1};
	}
}

// Calculates which city from thief @thiefId route is closest to city @cityId
int closestCity(int thiefId, int cityId) {
	
    // Setting the initial value - closest - to origin
	int best = adj[gang[thiefId].route[0].id][cityId];
	int bestI = 0;
	
	// Looks for an even closer city
    for(int i = 1; i < gang[thiefId].route.size(); i++){
		int from = gang[thiefId].route[i].id;
		if(best > adj[from][cityId]){
			best = adj[from][cityId];
			bestI = i;
		}
	}
	return bestI;
}

void clearSolution() {

	// Give back all the items you nasty gang of thiefs!
	for(int i = 0; i < items.size(); i++)
		items[i].thief = -1;

	// Clearing routes, resetting capacities and reinserting initial node (origin)
	for(int i = 0; i < gang.size(); i++){
		gang[i].capacity = 0;
		gang[i].route.clear();
		gang[i].route.push_back(Node()); 
	}

	gangCapacity = 0;

}

void joinNodes(int index, int i, int j) {

	//Takes items from @i and inserts in @j (same city, just repeated in the route)
	for(int k:gang[index].route[i].items)
		gang[index].route[j].items.push_back(k);

	// Updating weight and destroying repeated city after transfering items to @j
	gang[index].route[j].capacity += gang[index].route[i].capacity;
	gang[index].route.erase(gang[index].route.begin() + i);

}

// Invalid routes have repeating nodes (same city). This function makes them valid by merging
void fixRoute(int index) {

	// For each pair of nodes, if there is repetition JOIN!
	for(int i = 0; i < gang[index].route.size(); i++)
		for(int j = i + 1; j < gang[index].route.size(); j++){
			if(gang[index].route[i].id == gang[index].route[j].id ){
				// Adjusting indexes after sucessfull join
				joinNodes(index, i, j);
				i--;
				j--;
			}
		}
	
}

// Remove a given item
void removeItem(int id, int cityId, int itemPos){

	// If there is no item in this city don't even bother
    if(gang[id].route[cityId].items.size() == 0) return;

    int weight = items[gang[id].route[cityId].items[itemPos]].weight;
    int itemId = gang[id].route[cityId].items[itemPos];

	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	gang[id].route[cityId].capacity -= weight;
	gang[id].capacity -= weight;

	gangCapacity -= weight;
	items[itemId].thief = -1;
}

void printSolution() {

	std::cout << name << "_" << M << "_" << type << ":\n";
	for(int i = 0; i < gang.size(); i++){
		std::vector<int>I;
		int cap = 0;
		for(int j = 1; j < gang[i].route.size(); j++){
			if(j != gang[i].route.size() - 1)
				std::cout << gang[i].route[j].id + 1 << ",";
			else
				std::cout << gang[i].route[j].id + 1;
			for(int k = 0; k < gang[i].route[j].items.size(); k++)
				I.push_back(gang[i].route[j].items[k]);
		}
		std::cout << "\n";
		bool virg = false;
		for(auto j:I){
			if(!virg){
				std::cout << j + 1;
				virg = true;
			}
			else{
				std::cout << "," << j + 1;
			}
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

#endif