#include <bits/stdc++.h>
using namespace std;


// Below are the structres we use to represent the problem
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

// Problem variables
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

// Reading input
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

    // Give back the items that were taken from the selected city	
	for(int i = 0; i < gang[id].route[cityId].items.size(); i++){
        
        int pos = gang[id].route[cityId].items[i]; 
		
        gangCapacity -=  items[pos].weight;
		gang[id].capacity -=  items[pos].weight;
		items[pos].thief = -1;

	}
    
    // Remove it from the route
	gang[id].route.erase(gang[id].route.begin() + cityId);
	
	return p;
}

pair<int, int> removeItem(int id){

	int cont = 0;
	int cityId;
	
    do {
		
        cityId = rand() % (gang[id].route.size() - 1) + 1;
		cont++;
		if(cont > 100) return make_pair(-1, -1);

	} while(gang[id].route[cityId].items.size() <= 1);

	int itemPos = rand() % gang[id].route[cityId].items.size();
	int itemId = gang[id].route[cityId].items[itemPos];

	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	gang[id].route[cityId].capacity -= items[itemId].weight;
	gang[id].capacity -= items[itemId].weight;
	gangCapacity -= items[itemId].weight;
	items[itemId].thief = -1;

	return make_pair(cityId, itemId);
}

pair<int, int> addItem(int id){
	
    int cont = 0;
	int itemId;
	
    do {
		itemId = rand() % items.size();
	} while((items[itemId].thief != -1 || items[itemId].weight + gangCapacity > W)  && cont++ < 100);
    
    // If we couldn't pick an item with random after 100 iterations, go through the items vector to get one.
	if(cont >= 100){
		itemId = -1;
		for(int i = 0; i < items.size(); i++){
			if(items[i].thief == -1 && items[i].weight + gangCapacity <= W){
				itemId = i;
				break;
			}
		}
		if(itemId == -1) return make_pair(-1, -1); // Everything is already taken
	}

	// Updating capacities after getting the item
    gangCapacity += items[itemId].weight;
	gang[id].capacity += items[itemId].weight;
	items[itemId].thief = id;

    // If the item we took is in a city from our route, just add it to the Node item list
	for(int i = 0; i < gang[id].route.size(); i++){
		if(items[itemId].city == gang[id].route[i].id){
			gang[id].route[i].items.push_back(itemId);
			gang[id].route[i].capacity += items[itemId].weight;
			return make_pair(itemId, -1 * i); // Flag that indicated we took from a city within our route
		}
	}

	int bestPlace = closestCity(id, items[itemId].city);
    Node newItem = Node(items[itemId].city, {itemId}, items[itemId].weight);

	if(bestPlace)
		gang[id].route.insert(gang[id].route.begin() + bestPlace, newItem);
	else
		gang[id].route.push_back(newItem);

	return make_pair(itemId, bestPlace);
}

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

	//cerr << "Profit: " << total << endl;

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
			//cerr << i << " " << adj[a][b]/(vMax - v * capacity) << " " << (vMax - v * capacity) << endl;
		}

		// Coming back home
		capacity += end.capacity;
		currPenalty += adj[end.id][first.id]/(vMax - v * capacity);
		//cerr << i << " " << adj[end.id][first.id]/(vMax - v * capacity) << " " << (vMax - v * capacity) << endl;
		//cerr << endl;
		//cerr << adj[end.id][first.id]/(vMax - v * capacity) << endl;
	}

	total -= R * currPenalty;

	//cerr << "Penalty: " << R * currPenalty << endl;
	
	return total;
}

bool localSearchFirst(int type, int size){
	
	long double bestCost = cost(vMax, vMin, W, R);
	//cerr << bestCost << endl;
	bool isBetter = false;

	for(int i = 0; i < size; i++){
		
		/*Define a ordem de exploracao de vizinhanca dos ladroes*/
		vector<int>stepsOrder(gang.size());
		iota(stepsOrder.begin(), stepsOrder.end(), 0);
		random_shuffle(stepsOrder.begin(), stepsOrder.end());
		
		for(int j = 0; j < gang.size(); j++){
			int pos = stepsOrder[j];
			
			if(type == 0 && gang[pos].route.size() > 1){
				pair<int, int> swaped = opt2(stepsOrder[j]);
				
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else
					swap(gang[pos].route[swaped.first], gang[pos].route[swaped.second]);
			}
			else if(type == 1 && gang[pos].route.size() > 1){
				
                pair<pair<int, int>, pair<int, int>> swaped = opt4(pos);
				pair<int, int> swaped1 = swaped.first;
				pair<int, int> swaped2 = swaped.second;
				
				long double newCost = cost(vMax, vMin, W, R);
				
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					swap(gang[pos].route[swaped2.first], gang[pos].route[swaped2.second]);
					swap(gang[pos].route[swaped1.first], gang[pos].route[swaped1.second]);
				}
			}
			else if(type == 2 && gang[pos].route.size() > 1){
				
                pair<int, Node> removed = removeCity(pos);
				long double newCost = cost(vMax, vMin, W, R);
				
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					for(int k = 0; k < removed.second.items.size(); k++){
						gangCapacity +=  items[removed.second.items[k]].weight;
						gang[pos].capacity +=  items[removed.second.items[k]].weight;
						items[removed.second.items[k]].thief = pos;
					}
					gang[pos].route.insert(gang[pos].route.begin() + removed.first, removed.second);
				}
			}
			else if(type == 3 && gang[pos].route.size() > 1){

				pair<int, int> removed = removeItem(pos);
				
				if(removed.first == -1) continue;

				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					gang[pos].route[removed.first].items.push_back(removed.second);
					gang[pos].route[removed.first].capacity += items[removed.second].weight;
					gangCapacity += items[removed.second].weight;
					gang[pos].capacity += items[removed.second].weight;
					items[removed.second].thief = pos;
				}
			}
			else if(type == 4){

				pair<int, int> added = addItem(pos);

				if(added.first == -1) continue;

				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					gangCapacity -= items[added.first].weight;
					gang[pos].capacity -= items[added.first].weight;
					items[added.first].thief = -1;

					if(added.second < 0){
						added.second *= -1;
						gang[pos].route[added.second].items.pop_back();
						gang[pos].route[added.second].capacity -= items[added.first].weight;
					}
					else{
						if(added.second)
							gang[pos].route.erase(gang[pos].route.begin() + added.second);
						else
							gang[pos].route.pop_back();	
					}
				}
			}
			else if(type == 5 && gang[pos].route.size() > 1){

				pair<int, int> removed = removeItem(pos);
				pair<int, int> added = addItem(pos);

				long double newCost = cost(vMax, vMin, W, R);

				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					if(added.first != -1){
						gangCapacity -= items[added.first].weight;
						gang[pos].capacity -= items[added.first].weight;
						items[added.first].thief = -1;

						if(added.second < 0){
							added.second *= -1;
							gang[pos].route[added.second].items.pop_back();
							gang[pos].route[added.second].capacity -= items[added.first].weight;
						}
						else{
							if(added.second)
								gang[pos].route.erase(gang[pos].route.begin() + added.second);
							else
								gang[pos].route.pop_back();	
						}	
					}
					if(removed.first != -1){
						gang[pos].route[removed.first].items.push_back(removed.second);
						gang[pos].route[removed.first].capacity += items[removed.second].weight;
						gangCapacity += items[removed.second].weight;
						gang[pos].capacity += items[removed.second].weight;
						items[removed.second].thief = pos;
					}
				}
			}
		}
	}
	return isBetter;
}

bool VND(int size){
	bool isBetter = false; //Indica se melhorou ou nao
	int i = 0; //Inicia na vizinhanca 0 (opt2)
	while(i <= 5){
		//cerr << i << endl;
		//Faz size iterações naquela vizinhança
		if(localSearchFirst(i, size)){
			i = 0; //Se melhorou, volta para o início
			isBetter = true; //Aconteceu uma melhora na VND
		}
		else
			i++; //Passa para a próxima vizinhaça
	}
	return isBetter;
}

void clearSolution(){
	for(int i = 0; i < items.size(); i++)
		items[i].thief = -1;
	for(int i = 0; i < gang.size(); i++){
		gang[i].capacity = 0;
		gang[i].route.clear();
		gang[i].route.push_back(Node());
	}
	gangCapacity = 0;
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
				//cerr << "Fixed: " << i << " " << j << " " << gang[index].route[j].id << endl;
				joinNodes(index, i, j);
				i--;
				j--;
			}
		}
	}
}

void greedyInitialSolution(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0){

	vector<int>actualPos(numThiefs, 0);
	vector<bool>ended(numThiefs, false);

	long double v = (vMax - vMin)/W;

	clearSolution();

	double bestCost = 0.0;
	vector<Item>bestItems = items;
	vector<Thief>bestGang = gang;
	int bestGangCap = 0;

	bool hasOption = true;
	while(hasOption){
		hasOption = false;
		for(int i = 0; i < numThiefs; i++){
			if(ended[i])
				continue;
			int availableMoves = numMoves;
			while(availableMoves--){
				long double bestValue = numeric_limits<long double>::lowest();
				int bestItem = -1;

				for(int j = 0; j < M; j++){
					if(items[j].weight + gangCapacity > W || items[j].thief != -1)
						continue;
					
					long double goingCost = R * adj[actualPos[i]][items[j].city]/(vMax - v * gang[i].capacity);
					long double returnCost = R * adj[items[j].city][0]/(vMax - v * (gang[i].capacity + items[j].weight));
					
					if(!safe) returnCost = 0;

					//cout << actualPos[i] << " " << items[j].profit - goingCost - returnCost << " " << bestValue << " " << i <<" " << j << " " << items[j].city << endl;
					
					//cerr << i << " " << j << " " << items[j].profit - goingCost - returnCost << " " << bestValue << endl;
					if(items[j].profit - goingCost - returnCost > bestValue && rand() % 100 >= randomChance){
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
					//cerr << i << " " << cost(vMax, vMin, W, R) << endl;

					double actualCost = cost(vMax, vMin, W, R);
					if(actualCost > bestCost){
						bestCost = actualCost;
						bestItems = items;
						bestGang = gang;
						bestGangCap = gangCapacity;
					}
				}
			}
		}
	}

	items = bestItems;
	gang = bestGang;
	gangCapacity = bestGangCap;

	for(int i = 0; i < numThiefs; i++)
		fixRoute(i);
}

void GRASP(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0, int size = 300, int iter = 500){
	vector<Item> bestItem;
	vector<Thief> bestGang;
	int bestGangCap;
	long double bestCost = numeric_limits<long double>::lowest();

	while(iter--){
		greedyInitialSolution(numThiefs, safe, numMoves, randomChance);
		cerr << iter << " " << cost(vMax, vMin, W, R) << " ";
		VND(size);
		cerr << cost(vMax, vMin, W, R) << endl;
		long double newCost = cost(vMax, vMin, W, R);
		if(newCost > bestCost){
			bestItem = items;
			bestGang = gang;
			bestGangCap = gangCapacity;
			bestCost = newCost;
		}
	}
	items = bestItem ;
	gang = bestGang;
	gangCapacity = bestGangCap;
}
// Special 
void removeItem(int id, int cityId, int itemPos){

    if(gang[id].route[cityId].items.size() == 0) return;

    int weight = items[gang[id].route[cityId].items[itemPos]].weight;
    int itemId = gang[id].route[cityId].items[itemPos];

	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	gang[id].route[cityId].capacity -= weight;
	gang[id].capacity -= weight;
	gangCapacity -= weight;
	items[itemId].thief = -1;

}

void perturb(int factor) {

    for(int i=0; i<gang.size(); i++){
        
        if(gang[i].route.size() > 1) {
        
            int totalStolen = 0;
            for(int k=0; k<gang[i].route.size(); k++) totalStolen += gang[i].route[k].items.size();
                
            totalStolen /= 3;

            while(totalStolen--) {
            
                int pos = rand()%(gang[i].route.size()-1) + 1; // Avoid getting city zero
                if(gang[i].route[pos].items.size() > 1) {
                    int itemId = rand()%gang[i].route[pos].items.size();
                    removeItem(i,pos,itemId);
                }
                else {
                   removeItem(i,pos,0);
                   gang[i].route.erase(gang[i].route.begin() + pos);
                }
            }
        }
    }
}

void ILS(int numThiefs, bool safe = false, int numMoves = 1, int size = 300, int iter = 500) {
    

	vector<Item> bestItem;
	vector<Thief> bestGang;
	int bestGangCap = 0;
	long double bestCost = numeric_limits<long double>::lowest();
	greedyInitialSolution(numThiefs, safe, numMoves, 0);
    int factor = 1;
    while(iter--) {
    
        //cerr << iter+1 << " " << cost(vMax, vMin, W, R) << " "; 
        perturb(factor); // Factor is a dummy for a while, later on it can influence the level of perturbation
        //cerr << cost(vMax, vMin, W,R) << " "; 
        VND(size);
        long double actualCost = cost(vMax, vMin, W, R);
        //cerr << actualCost << endl; 

        if( actualCost  > bestCost ) {
            bestItem = items;
            bestGang = gang;
            bestGangCap = gangCapacity;
            bestCost = actualCost;
        }
    }    

    items = bestItem;
    gang = bestGang;
    gangCapacity = bestGangCap;
}


int main(int argc, char **argv) {
	srand(time(NULL));

	readInstance(name, type, V, M, W, vMin, vMax, R);
	int numThiefs = atoi(argv[1]);
	bool safe = atoi(argv[2]);
	int numMoves = atoi(argv[3]);
	int randomChance = atoi(argv[4]);
	int size = atoi(argv[5]);
	int iter = atoi(argv[6]);
	
	gang.resize(numThiefs);


    ILS(numThiefs, safe, numMoves, size, iter);
	//GRASP(numThiefs, safe, numMoves, randomChance, size, iter);

	// greedyInitialSolution(numThiefs, atoi(argv[2]), atoi(argv[3]));
	// VND(argv[5]);
	

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
		cout << endl;
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
    cerr << name << endl;
	cerr << "Value: " << fixed << setprecision(2) << cost(vMax, vMin, W, R) << endl;
	cerr << "Weight: " << gangCapacity << " Max Capacity: " << W << endl;
	cerr << "Parameters: ";
	cerr << "nThiefs " << numThiefs << " safe " << safe << " nMoves " << numMoves << " %rand " << randomChance << " neighborhood size " << size << " iterations " << iter << endl;

}
