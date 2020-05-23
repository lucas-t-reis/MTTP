#ifndef MHEURISTICS_H
#define MHEURISTICS_H

/* ------------------ NEIGHBORHOODS ------------------ */

// Exchange two cities from the route randomly
std::pair<int, int> opt2(int id) {

	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	
	std::swap(gang[id].route[a], gang[id].route[b]);
	
	return std::make_pair(a, b); 
}

// Exchange four cities from the route randomly
std::pair<std::pair<int, int>, std::pair<int , int> > opt4(int id) {
	
	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	int c = rand() % (gang[id].route.size() - 1) + 1;
	int d = rand() % (gang[id].route.size() - 1) + 1;
	
	std::swap(gang[id].route[a], gang[id].route[b]);
	std::swap(gang[id].route[c], gang[id].route[d]);
	
	auto p1 = std::make_pair(a, b);
	auto p2 = std::make_pair(c, d);

	return std::make_pair(p1,p2); 
}

// Removes a city from the Thiefs route
std::pair<int, Node> removeCity(int id){
	
	// Pick a random city
    int cityId = rand() % (gang[id].route.size() - 1) + 1;
	auto p = std::make_pair(cityId, gang[id].route[cityId]); //Salva o indice no vetor de rotas e os dados do nó

    // Return the items taken from that city - if there is any
	for(int i = 0; i < gang[id].route[cityId].items.size(); i++){
        
		// Item index
        int pos = gang[id].route[cityId].items[i]; 
		
		// The item removed is free to be taken by other thief now.
        // Also, the weight it had on the @globalCapacity is now removed - since we returned it.
        gangCapacity -=  items[pos].weight;
		gang[id].capacity -=  items[pos].weight;
		items[pos].thief = -1;

	}
    
    // Remove it from the route
	gang[id].route.erase(gang[id].route.begin() + cityId);
	
    // Returns index and node
	return p; 
}

// Removes an item from the Thiefs route
std::pair<int, int> removeItem(int id) {

	int cont = 0;
	int cityId;
	
	// Draw a city with more than 1 item, trying at most 100 times to do so.
    do {

        cityId = rand() % (gang[id].route.size() - 1) + 1;
		cont++;
		if(cont > 100) return std::make_pair(-1, -1); // Flag indicating it wasn't possible to draw a city with at least 1 item ( pair.first = -1 )

	} while(gang[id].route[cityId].items.size() <= 1);

	// Pick a random item from the chosen city
	int itemPos = rand() % gang[id].route[cityId].items.size();
	int itemId = gang[id].route[cityId].items[itemPos];

	// Remove it from the Thiefs route
	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	
	// Update current capacity and global capacity
	gang[id].route[cityId].capacity -= items[itemId].weight;
	gang[id].capacity -= items[itemId].weight;
	gangCapacity -= items[itemId].weight;
	
	// Mark the item as not taken
	items[itemId].thief = -1;

	return std::make_pair(cityId, itemId);
}

// Adds an item to a Thief route
std::pair<int, int> addItem(int id) {
	
    int cont = 0;
	int itemId;

    // Pick an item that doens't overflow current @globalCapacity and that hasn't been taken. Try that at most 100 times
    do {
		itemId = rand() % items.size();
	} while((items[itemId].thief != -1 || items[itemId].weight + gangCapacity > W)  && cont++ < 100);
    
    // If we couldn't pick an item randomly after 100 trials, go through the @items vector and get the first avaliable.
	if(cont >= 100){
		itemId = -1;
		for(int i = 0; i < items.size(); i++){
			if(items[i].thief == -1 && items[i].weight + gangCapacity <= W){
				itemId = i;
				break;
			}
		}

        // If everything is already taken, return fail flag ( pair.first = -1 )
		if(itemId == -1) return std::make_pair(-1, -1); 
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
			
            // Flag indicating that we took the item from a city within our route ( pair.second = -idCity )
            return std::make_pair(itemId, -1 * i); 
		
        }
	}

	// Look for the best place to fit the new city - if the item comes from a not visited city
	int bestPlace = closestCity(id, items[itemId].city);
    Node newItem = Node(items[itemId].city, {itemId}, items[itemId].weight); // Creating the new city node to go in route 

	//  If the best option isn't the origin
	if(bestPlace)
		gang[id].route.insert(gang[id].route.begin() + bestPlace, newItem); // Add right before the best option
	else
		gang[id].route.push_back(newItem); // Add in the end of the route. Making it be "before" the origin - remmember the thief has to return in the end, hence the circulatiry

	return std::make_pair(itemId, bestPlace);
}


/* ------------------ COST FUNCTION ------------------ */

long double cost(long double vMax, long double vMin, int W, long double R) {

	long double total = 0.0;            
	long double currPenalty = 0.0;      // Backpack fee cost
	long double v = (vMax - vMin)/W;    // Defined in the problem description
	
	for(int i=0; i < gang.size(); i++){

		int capacity = 0; // Total weight carried by thief @i from at a certain route

		auto first = gang[i].route.front();
		auto end = gang[i].route.back(); 

		// Go until the penultimate city (Avoids seg. fault) 
		for(int j = 0; j < gang[i].route.size() - 1; j++){
			
			capacity += gang[i].route[j].capacity;

			int a = gang[i].route[j].id;        // Current city
			int b = gang[i].route[j + 1].id;    // Next city

			// Sum profit from items in the next city - first city has no item by definition and we go until the penultimate city.
			for(int k = 0; k < gang[i].route[j + 1].items.size(); k++)
				total += items[gang[i].route[j + 1].items[k]].profit;
			
			currPenalty += adj[a][b]/(vMax - v * capacity); // Adds the cost of going from @a to @b in the current fee
		}

		// Coming back home, adding weight carried from last city and final backpack fee
		capacity += end.capacity;                                   
		currPenalty += adj[end.id][first.id]/(vMax - v * capacity);
	}

	total -= R * currPenalty;

	return total;

}

bool localSearchFirst(int type, int size) {

	long double bestCost = cost(vMax, vMin, W, R); 
	bool isBetter = false;

	// For a given neighborhood size
	for(int i = 0; i < size; i++){
		
        // Define a random order of neighborhood exploration, aiming to diversify the solutions.
		std::vector<int>stepsOrder(gang.size());                
		std::iota(stepsOrder.begin(), stepsOrder.end(), 0);     
		random_shuffle(stepsOrder.begin(), stepsOrder.end());
		
		for(int j = 0; j < gang.size(); j++) {

            // Current thief index
			int pos = stepsOrder[j]; 

			/*    
                    NEIGHBORHOODS

			        0 = opt2 
			        1 = opt4 
			        2 = remove city
			        3 = remove item  
			        4 = add item
			        5 = exchange item   
            */

			// Explore a given neighborhood
			if(type == 0 && gang[pos].route.size() > 1){ // Only swaps if there is more than one city in the route
				
                std::pair<int, int> swaped = opt2(stepsOrder[j]);
				
				// Checks if the cost improved and updates
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
                // If things weren't good, undo changes
				else std::swap(gang[pos].route[swaped.first], gang[pos].route[swaped.second]);
			} 
			else if(type == 1 && gang[pos].route.size() > 1){ 
				
                // Swap
                std::pair<std::pair<int, int>, std::pair<int, int>> swaped = opt4(pos);
				std::pair<int, int> swaped1 = swaped.first;
				std::pair<int, int> swaped2 = swaped.second;
				
				long double newCost = cost(vMax, vMin, W, R);
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
                // If things weren't good, undo changes
				else {
					std::swap(gang[pos].route[swaped2.first], gang[pos].route[swaped2.second]);
					std::swap(gang[pos].route[swaped1.first], gang[pos].route[swaped1.second]);
				}
			}
			else if(type == 2 && gang[pos].route.size() > 1){
				
                std::pair<int, Node> removed = removeCity(pos);

				long double newCost = cost(vMax, vMin, W, R);
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
                // Take the items back and re-update the capacities
				else {

					for(int k = 0; k < removed.second.items.size(); k++){
						gangCapacity +=  items[removed.second.items[k]].weight;
						gang[pos].capacity +=  items[removed.second.items[k]].weight;
						items[removed.second.items[k]].thief = pos;
					}
					// Putting the city back in the route
					gang[pos].route.insert(gang[pos].route.begin() + removed.first, removed.second);
				}
			}
			else if(type == 3 && gang[pos].route.size() > 1){

				std::pair<int, int> removed = removeItem(pos);
				
				// Couldn't remove
				if(removed.first == -1) continue;

				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
                // Undo changes
				else {
					gang[pos].route[removed.first].items.push_back(removed.second);
					gang[pos].route[removed.first].capacity += items[removed.second].weight;
					gangCapacity += items[removed.second].weight;
					gang[pos].capacity += items[removed.second].weight;
					items[removed.second].thief = pos;
				}
			}
			else if(type == 4){


				std::pair<int, int> added = addItem(pos);

				// No valid items to add
				if(added.first == -1) continue;

				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					// Updates capacities
					gangCapacity -= items[added.first].weight;
					gang[pos].capacity -= items[added.first].weight;
					items[added.first].thief = -1;

					// If the city was already in the route (flag: pair.second < 0 )
					if(added.second < 0){
						added.second *= -1;
						gang[pos].route[added.second].items.pop_back();
						gang[pos].route[added.second].capacity -= items[added.first].weight;
					}
                    // Otherwise, remove the whole city - added because of 1 item - from the route
					else {
						if(added.second)
							gang[pos].route.erase(gang[pos].route.begin() + added.second);
						else
							gang[pos].route.pop_back();	
					}
				}
			}
			else if(type == 5 && gang[pos].route.size() > 1){

                // Removes a random item and adds a new one right after.
				std::pair<int, int> removed = removeItem(pos);
				std::pair<int, int> added = addItem(pos);

				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
                // Undoing changes
				else{
					// If it was possible to add an item
					if(added.first != -1){

						// Update statuses
						gangCapacity -= items[added.first].weight;
						gang[pos].capacity -= items[added.first].weight;
						items[added.first].thief = -1;

						// If the city is already in the current route, you just need to remove the item.
						if(added.second < 0){
							added.second *= -1;
							gang[pos].route[added.second].items.pop_back();
							gang[pos].route[added.second].capacity -= items[added.first].weight;
						}
                        // Otherwise remove the city
						else {
							if(added.second)
								gang[pos].route.erase(gang[pos].route.begin() + added.second);
							else
								gang[pos].route.pop_back();	
						}	
					}
					// If something was removed in the attempt for a better solution
					if(removed.first != -1){
						// Updates capacities and take back the removed item
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


/* ------------------ METAHEURISTICS ------------------ */

// Variable Neighborhood Descent -- Using 6 neighborhoods of the same size. By default the initial one is 0 (opt2)
bool VND(int size) {

	bool isBetter = false;
	int i = 0; 

	while(i <= 5 && !timeLimit(600)){

		// Iterate in current neighborhood. Improving means going back to first neighborhood
		if(localSearchFirst(i, size)){
			i = 0;  // 
			isBetter = true; 
		}
        // No improvements asks for diversity. Therefore we go to different neighborhoods
		else
			i++; 
	}
	return isBetter; 
}

void greedyInitialSolution(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0){

	std::vector<int>actualPos(numThiefs, 0);
	std::vector<bool>ended(numThiefs, false);   

	long double v = (vMax - vMin)/W; 

	clearSolution(); 

	double bestCost = 0.0;
	std::vector<Item>bestItems = items;
	std::vector<Thief>bestGang = gang;
	int bestGangCap = 0;

	bool hasOption = true; 
	while(hasOption) {
		
		hasOption = false;

		for(int i = 0; i < numThiefs; i++){
			
			// If the thief @i returned, go to the next one
			if(ended[i]) continue;
			
			// Max moves for each thief per iteration
			int availableMoves = numMoves;

			while(availableMoves--){

				// Initializes with -INFINITY to allow a lot of exploration. 
                // If no @bestItem was found to put in the current thief route, returns the flag -1
				long double bestValue = std::numeric_limits<long double>::lowest();
				int bestItem = -1; 

				for(int j = 0; j < M; j++){
					
					// Item taken or too heavy? Keep going...
					if(items[j].weight + gangCapacity > W || items[j].thief != -1) continue;
					
					// Cost of taking the item from its respective city and coming back to the beginning
					long double goingCost = R * adj[actualPos[i]][items[j].city]/(vMax - v * gang[i].capacity);
					long double returnCost = R * adj[items[j].city][0]/(vMax - v * (gang[i].capacity + items[j].weight));
					
					// Unsafe mode means we ignore the cost of comming back. It may lead to negative costs but also allows for greater exploration
                    if(!safe) returnCost = 0;

                    // If adding this item is better than the best found up until now, put it on the route.
                    // There is also a chance to ignore the current item, no matter how good/bad it is.
					if(items[j].profit - goingCost - returnCost > bestValue && rand() % 100 >= randomChance){
							bestValue = items[j].profit - goingCost - returnCost;
							bestItem = j;
							hasOption = true; 
					}

				}

				// No item worth the trouble for this Thief
				if(bestItem == -1){
					actualPos[i] = 0;   // Goes back to the beginnig
					ended[0] = true;    // Flag indicating this Thief is done stealing
				}
				else {

                    // No cities on the route or the current item city isn't the last on the route means we can add a city
					if(gang[i].route.empty() || gang[i].route.back().id != items[bestItem].city){
						gang[i].route.push_back(Node());
						gang[i].route.back().id = items[bestItem].city;
						gang[i].route.back().capacity = 0;
					}

                    // Adds the item weight in the last city and update its status as taken
					gang[i].route.back().items.push_back(bestItem);
					gang[i].route.back().capacity += items[bestItem].weight;
					gang[i].capacity += items[bestItem].weight;
					gangCapacity += items[bestItem].weight;
					items[bestItem].thief = i;
					actualPos[i] = items[bestItem].city;
				
					// Calculates current solution cost and updates if benefitial
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

    // This greedy can create invalid solutions (repeated cities in a route). 
    // To be able to use those solutions, fix them first.
	for(int i = 0; i < numThiefs; i++)
		fixRoute(i);
}

// Greedy Randomized Adaptive Search Procedure -- using a fixed random rate and a VND instead of a vanilla local search 
void GRASP(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0, int size = 300, int iter = 500) {
	
	start = std::chrono::system_clock::now();

	std::vector<Item> bestItem;
	std::vector<Thief> bestGang;

	int bestGangCap;
	long double bestCost = std::numeric_limits<long double>::lowest();

	while(iter-- && !timeLimit(600)) {

		greedyInitialSolution(numThiefs, safe, numMoves, randomChance);
		VND(size);

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

// ILS auxiliar function
void perturb(double factor) {
        
    // Ceiling to the factor in case it starts to grow too much
	if(factor >= 0.4) factor = 0.5;

    for(int i=0; i<gang.size(); i++){

        if(gang[i].route.size() > 1){

            int totalStolen = 0;
            for(int k=0; k<gang[i].route.size(); k++) totalStolen += gang[i].route[k].items.size();
            
			// How many items should we keep after the "shakedown" from perturb
            totalStolen = (int) ((0.40 + factor) * totalStolen);

			// Remove one random item at a time
            while(totalStolen--) {
                
				int pos = rand() % (gang[i].route.size() - 1) + 1; // + 1 Avoids drawing city zero ( by definition it has no item )
                
				// If the random item is not the only one in the city inventory, take care to remove just it
				if(gang[i].route[pos].items.size() > 1) { 
                    int itemId = rand()%gang[i].route[pos].items.size();
                    removeItem(i,pos,itemId);
                }
                // If there is just one item, you might as well remove the whole city from the route
                else { 
                   removeItem(i, pos, 0);
                   gang[i].route.erase(gang[i].route.begin() + pos);
                }

            }
        }

    }
}

// Iterated Local Search with dinamic perturbation and VND as local search
void ILS(int numThiefs, bool safe = false, int numMoves = 1, int size = 100, int iter = 300) {
       
	start = std::chrono::system_clock::now(); 
	
	std::vector<Item> bestItem;
	std::vector<Thief> bestGang;
	
	int bestGangCap = 0;
	long double bestCost = std::numeric_limits<long double>::lowest(); 
	
	// Initial solution without a random
	greedyInitialSolution(numThiefs, safe, numMoves, 0);
    
	double factor = 0;      // Initialing with a low perturbation
    int ct = 0;             // Non-improving iterations
    bool improved = false;  
    
	while(iter-- && !timeLimit(600)) {

        ct++;
        improved = false;

        perturb(factor); 
        VND(size);
        
        // Updating solution if it improved and resetting the Non-improved counter
		long double actualCost = cost(vMax, vMin, W, R);
        if( actualCost  > bestCost ) { 
            improved = true;
            factor = 0;
            ct = 0;
            bestItem = items;
            bestGang = gang;
            bestGangCap = gangCapacity;
            bestCost = actualCost;
        }
        // Not improving after 50 iterations triggers a factor increase (more severe perturbations willl be done)
        if(ct >= 50 && !improved)
            factor += 0.05;
    }    

    items = bestItem;
    gang = bestGang;
    gangCapacity = bestGangCap;
}

#endif