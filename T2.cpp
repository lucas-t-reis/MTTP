#include <bits/stdc++.h>
using namespace std;


/*** Estruturas usadas para representar o problema ***/

//Cidades
struct City{
	City(){}
	City(long double x_, long double y_) : x(x_), y(y_) {}
	long double x, y; //Coordenadas das cidades
};

//Cidade dentro da rota do ladrão (um nó da rota)
struct Node{
	Node(): id(0), capacity(0){}
	Node(int id_, const vector<int> &items_, int capacity_) : id(id_), items(items_), capacity(capacity_){}
	int id; //ID da cidade do nó
	vector <int> items; //Indices dos items que esse ladrao pegou nessa cidade
	int capacity; //Soma do peso dos items que o ladrao pegou nessa cidade
};

//Itens que podem ser roubados
struct Item{
	Item(){}
	Item(int profit_, int weight_, int thief_, int city_) : profit(profit_), weight(weight_), thief(thief_),
	city(city_){}
	int profit; //Valor do item
	int weight; //Peso do item
	int thief; //Qual ladrão pegou o item (-1 significa nenhum)
	int city; //Qual a cidade do item
};

//Ladrões
struct Thief{
	Thief():capacity(0), route(vector<Node>(1, Node())){}
	Thief(const vector<Node> &route_, int capacity_) : route(route_), capacity(capacity_) {}
	vector <Node> route; //Rota do ladrão, sem repetir o inicio e o fim
	int capacity; //Peso que aquele ladrao carrega naquele momento
};

// Variáveis do problema
vector <City> cities; //Vetor de cidades
vector <Item> items; //Vetor de items
vector <Thief> gang; //Vetor de ladroes
vector <vector <int>> adj; //Matriz de adjacencia das cidades
int gangCapacity = 0; //Peso atual que os ladrões carregam juntos


string name, type; //Nome e tipo da instancia
int V, M, W; //numero de cidades, numero de items e capacidade da gangue de ladroes
long double vMin, vMax, R; //Velocidade minima, velocidade maxima e taxa de aluguel

chrono::time_point<chrono::system_clock> start; //Tempo de inicio da heuristica

//Checa se o tempo limite, em segundos, foi atingido
bool timeLimit(long double maxSeconds){
	return chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now() - start).count() > maxSeconds;
}

//Calcula a distancia entre as cidades a e b
int dist(int a, int b){
	return ceil(sqrt((cities[a].x - cities[b].x) * (cities[a].x - cities[b].x) + (cities[a].y - cities[b].y) * (cities[a].y - cities[b].y))); 
}

// Le a entrada numerica
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

//Le a instancia
void readInstance(string &name, string &type, int &V, int &M, int &W, long double &vMin, long double &vMax, long double &R){
	// Lendo nome e tipo	
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

	// Lendo a parte numerica dos dados
	int k=0;
	int temp[3] = {0};
	long double tempDouble[3] = {0.0};

	while(k<3) readNum(temp[k++],tempDouble[0],true);
	
	V = temp[0]; //Num de cidades
	M = temp[1]; //Num de items
	W = temp[2]; //Capacidade maxima

	k=0;
	while(k<3) readNum(temp[0],tempDouble[k++], false);
	
	vMin = tempDouble[0]; //Velocidade minima
	vMax = tempDouble[1]; //Velocidade maxima
	R = tempDouble[2]; //Taxa de aluguel

	// Ignoring last line from header
	getline(cin, line);

	// Copying city coordinates
	getline(cin, line);
	
	cities.resize(V); //Cresce o vetor de cidade
	
	//Le as cidades
	for(int i = 0; i < V; i++){
		int id;
		long double x, y;

		cin >> id >> x >> y;
	
		cities[i] = {x, y};
	}

	//Cria a matriz de adjacencia
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
	
	items.resize(M); //Aumenta o vetor de itens
	
	//Le os itens
	for(int i = 0; i < M; i++){
		int id, p, w, idCity; 

		cin >> id >> p >> w >> idCity;
	
		items[i] = {p, w, -1, idCity - 1};
	}
}

/*Calcula qual cidade na rota do ladrão está
mais próxima da cidade em questão*/
int closestCity(int thiefId, int cityId){
	
    //A mais proxima comeca com a origem
	int best = adj[gang[thiefId].route[0].id][cityId];
	int bestI = 0;
	
	//Busca uma opcao melhor
    for(int i = 1; i < gang[thiefId].route.size(); i++){
		int from = gang[thiefId].route[i].id;
		if(best > adj[from][cityId]){
			best = adj[from][cityId];
			bestI = i;
		}
	}
	return bestI; //Retorna a melhor
}

/***Vizinhanças***/

/* Troca duas cidades aleatorias da rota de lugar*/
pair<int, int> opt2(int id){

	//Sorteia
	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	
	//Troca
	swap(gang[id].route[a], gang[id].route[b]);
	
	return make_pair(a, b); //Retorna qual foi o par
}

/* Troca quatro cidades aleatorias da rota de lugar*/
pair<pair<int, int>, pair<int , int>> opt4(int id){
	
	//Sorteia
	int a = rand() % (gang[id].route.size() - 1) + 1;
	int b = rand() % (gang[id].route.size() - 1) + 1;
	int c = rand() % (gang[id].route.size() - 1) + 1;
	int d = rand() % (gang[id].route.size() - 1) + 1;
	
	//Troca
	swap(gang[id].route[a], gang[id].route[b]);
	swap(gang[id].route[c], gang[id].route[d]);
	
	auto p1 = make_pair(a, b);
	auto p2 = make_pair(c, d);

	return make_pair(p1,p2); //Retorna quais foram os pares
}


/*Remove uma cidade inteira da rota de um ladrao*/
pair<int, Node> removeCity(int id){
	
	//Sorteia uma cidade
    int cityId = rand() % (gang[id].route.size() - 1) + 1;
	auto p = make_pair(cityId, gang[id].route[cityId]); //Salva o indice no vetor de rotas e os dados do nó

    // Devolve os itens da cidade retirada
	for(int i = 0; i < gang[id].route[cityId].items.size(); i++){
        
		//Indice do item em questao
        int pos = gang[id].route[cityId].items[i]; 
		
		//Reduz as capacidades e marca o item como vago
        gangCapacity -=  items[pos].weight;
		gang[id].capacity -=  items[pos].weight;
		items[pos].thief = -1;

	}
    
    // Remove it from the route
	gang[id].route.erase(gang[id].route.begin() + cityId);
	
	return p; //Retorna o par indice, nodo
}


/*Remove um item da rota de um ladrao*/
pair<int, int> removeItem(int id){

	int cont = 0;
	int cityId;
	
	/*Sorteia uma cidade com mais de um item (para simplificar a vizinhanca),
	com um limite de ate 100 sorteios sem sucesso*/
    do{
        cityId = rand() % (gang[id].route.size() - 1) + 1;
		cont++;
		if(cont > 100) return make_pair(-1, -1); //Informa que não foi possivel sortear (a flag é first = -1)

	}while(gang[id].route[cityId].items.size() <= 1);

	//Sorteia um dos items da cidade
	int itemPos = rand() % gang[id].route[cityId].items.size();
	int itemId = gang[id].route[cityId].items[itemPos];

	//Remove o item da cidade da rota do ladrão
	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	
	//Desconta das capacidades
	gang[id].route[cityId].capacity -= items[itemId].weight;
	gang[id].capacity -= items[itemId].weight;
	gangCapacity -= items[itemId].weight;
	
	//Desmarca o item
	items[itemId].thief = -1;

	return make_pair(cityId, itemId); //Retorna o par id da cidade, dados do item
}


/*Adiciona um item à rota de um ladrão*/
pair<int, int> addItem(int id){
	
    int cont = 0;
	int itemId;

	//Sorteia um item que respeita a capacidade e que nao tenho sido pego, com um limte de 100 sorteios
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
		if(itemId == -1) return make_pair(-1, -1); // Everything is already taken (flag is first = -1)
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
			return make_pair(itemId, -1 * i); // Flag that indicated we took from a city within our route (second = -idCity)
		}
	}

	//Busca o melhor lugar para encaixar a nova cidade
	int bestPlace = closestCity(id, items[itemId].city);
    Node newItem = Node(items[itemId].city, {itemId}, items[itemId].weight); //Cria o novo nodo da rota

	//Se a melhor opcao nao é a origem
	if(bestPlace)
		gang[id].route.insert(gang[id].route.begin() + bestPlace, newItem); //Acrescenta antes da melhor opcao
	else
		gang[id].route.push_back(newItem); //Acrescenta no fim do vetor ("antes da origem")

	return make_pair(itemId, bestPlace); //Retorna o par id do item, local onde a cidade foi inserida
}

/*Função objetivo, que calcula o valor da solução atual*/
long double cost(long double vMax, long double vMin, int W, long double R){

	long double total = 0.0; //Resultado final
	long double currPenalty = 0.0; //Penalidade (custo do aluguel)
	long double v = (vMax - vMin)/W; // Defined in the problem description
	
	for(int i=0; i < gang.size(); i++){

		int capacity = 0; //Capacidade total do ladrão naquele ponto da rota

		auto first = gang[i].route.front(); //Primeira cidade da rota
		auto end = gang[i].route.back(); //Ultima cidade da rota

		//O for vai ate a penultima cidade (para evitar segmentation fault)
		for(int j = 0; j < gang[i].route.size() - 1; j++){
			
			capacity += gang[i].route[j].capacity; //Soma a capacidade adquirida na cidade na capacidade total atual

			int a = gang[i].route[j].id; //Cidade atual
			int b = gang[i].route[j + 1].id; //Proxima cidade

			//Soma o lucro dos items da proxima cidade (pois a zero não tem items e vamos até à penultima no for apenas)
			for(int k = 0; k < gang[i].route[j + 1].items.size(); k++)
				total += items[gang[i].route[j + 1].items[k]].profit;
			
			currPenalty += adj[a][b]/(vMax - v * capacity); //Soma o custo da ida na penalidade
		}

		// Coming back home
		capacity += end.capacity; //Acrescimo de capacidade na ultima cidade
		currPenalty += adj[end.id][first.id]/(vMax - v * capacity); //Soma a volta na penalidade
	}

	total -= R * currPenalty; //Multiplica a penalidade pela taxa e abate do total

	return total; //Retorna o total
}

bool localSearchFirst(int type, int size){

	long double bestCost = cost(vMax, vMin, W, R); //Melhor valor atual
	bool isBetter = false; //Indice se houve melhora

	//Repete um numero de vezes igual ao tamanho da vizinhanca 
	for(int i = 0; i < size; i++){
		
		/*Define a ordem de exploracao de vizinhanca dos ladroes de
		forma aleatória, de modo a diversificar as soluções*/
		vector<int>stepsOrder(gang.size()); //Ordem dos passos
		iota(stepsOrder.begin(), stepsOrder.end(), 0); //Preenche o vetor de 0 a n - 1
		random_shuffle(stepsOrder.begin(), stepsOrder.end()); //Embaralha o vetor da ordem
		
		for(int j = 0; j < gang.size(); j++){
			int pos = stepsOrder[j]; //Indice do ladrao atual
			
			/***  Vizinhancas    ***/
			/***0 = opt2         ***/
			/***1 = opt4         ***/
			/***2 = remove cidade***/
			/***3 = remove item  ***/
			/***4 = adiciona item***/
			/***5 = troca item   ***/
			/***********************/

			/*Faz a exploracao na vizinhaca indicada*/

			if(type == 0 && gang[pos].route.size() > 1){ //Só troca se há mais de uma cidade na rota
				//Troca
				pair<int, int> swaped = opt2(stepsOrder[j]);
				
				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else
					swap(gang[pos].route[swaped.first], gang[pos].route[swaped.second]); //Destroca
			} 
			else if(type == 1 && gang[pos].route.size() > 1){ //Só troca se há mais de uma cidade na rota
				
				//Troca
                pair<pair<int, int>, pair<int, int>> swaped = opt4(pos);
				pair<int, int> swaped1 = swaped.first;
				pair<int, int> swaped2 = swaped.second;
				
				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					//Destroca
					swap(gang[pos].route[swaped2.first], gang[pos].route[swaped2.second]);
					swap(gang[pos].route[swaped1.first], gang[pos].route[swaped1.second]);
				}
			}
			else if(type == 2 && gang[pos].route.size() > 1){ //Só remove se há mais de uma cidade na rota
				
				//Remove
                pair<int, Node> removed = removeCity(pos);

				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
                if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					//Marca os itens de volta com do ladrão e atualiza as capacidades
					for(int k = 0; k < removed.second.items.size(); k++){
						gangCapacity +=  items[removed.second.items[k]].weight;
						gang[pos].capacity +=  items[removed.second.items[k]].weight;
						items[removed.second.items[k]].thief = pos;
					}
					//Reinsere na rota
					gang[pos].route.insert(gang[pos].route.begin() + removed.first, removed.second);
				}
			}
			else if(type == 3 && gang[pos].route.size() > 1){ //Só remove se há mais de uma cidade na rota

				//Remove
				pair<int, int> removed = removeItem(pos);
				
				//Não foi possível remover
				if(removed.first == -1) continue;

				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					//Atualiza as capacidades e marca como do ladrão novamente
					gang[pos].route[removed.first].items.push_back(removed.second);
					gang[pos].route[removed.first].capacity += items[removed.second].weight;
					gangCapacity += items[removed.second].weight;
					gang[pos].capacity += items[removed.second].weight;
					items[removed.second].thief = pos;
				}
			}
			else if(type == 4){

				//Adiciona
				pair<int, int> added = addItem(pos);

				//Não há items válidos para adicionar
				if(added.first == -1) continue;

				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					//Atualiza as capacidades
					gangCapacity -= items[added.first].weight;
					gang[pos].capacity -= items[added.first].weight;
					items[added.first].thief = -1;

					//Se a cidade já estava na rota (flag: second < 0), apenas remove o item extra
					if(added.second < 0){
						added.second *= -1;
						gang[pos].route[added.second].items.pop_back();
						gang[pos].route[added.second].capacity -= items[added.first].weight;
					}
					else{ // Senão, remove a cidade inteira da rota
						if(added.second)
							gang[pos].route.erase(gang[pos].route.begin() + added.second);
						else
							gang[pos].route.pop_back();	
					}
				}
			}
			else if(type == 5 && gang[pos].route.size() > 1){

				//Remove um item aleatório e adiociona outro logo após ("troca os items")
				pair<int, int> removed = removeItem(pos);
				pair<int, int> added = addItem(pos);

				//Checa se melhorou e atualiza
				long double newCost = cost(vMax, vMin, W, R);
				if(bestCost < newCost){
					bestCost = newCost;
					isBetter = true;
				}
				else{
					//Se foi possivel adicionar
					if(added.first != -1){

						//Atualiza as capacidades
						gangCapacity -= items[added.first].weight;
						gang[pos].capacity -= items[added.first].weight;
						items[added.first].thief = -1;

						//Se a cidade já esta na rota, apenas remove o item
						if(added.second < 0){
							added.second *= -1;
							gang[pos].route[added.second].items.pop_back();
							gang[pos].route[added.second].capacity -= items[added.first].weight;
						}
						else{ //senao, remove a cidade
							if(added.second)
								gang[pos].route.erase(gang[pos].route.begin() + added.second);
							else
								gang[pos].route.pop_back();	
						}	
					}
					//Se foi possível remover
					if(removed.first != -1){
						//Atualiza as capacidades e readiciona o item removido
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
	return isBetter; //Informa se houve melhora
}

/*Heurística VND (variable Neighborhood Descent), com 6 vizinhancas
de mesmo tamanho*/
bool VND(int size){
	bool isBetter = false; //Indica se melhorou ou nao
	int i = 0; //Inicia na vizinhanca 0 (opt2)
	while(i <= 5 && !timeLimit(600)){
		//Faz size iterações naquela vizinhança
		if(localSearchFirst(i, size)){
			i = 0; //Se melhorou, volta para o início
			isBetter = true; //Aconteceu uma melhora na VND
		}
		else
			i++; //Passa para a próxima vizinhaça
	}
	return isBetter; //Houve melhora na VND
}

/*Limpa uma solução*/
void clearSolution(){
	//Desmarca os items dos ladrões
	for(int i = 0; i < items.size(); i++)
		items[i].thief = -1;
	//Limpa as rotas e capacidades
	for(int i = 0; i < gang.size(); i++){
		gang[i].capacity = 0;
		gang[i].route.clear();
		gang[i].route.push_back(Node()); //Reinsere a cidade inicial
	}
	gangCapacity = 0;
}

//Junta dois nodos repetidos em uma rota
void joinNodes(int index, int i, int j){
	//Insere os itens de i em j
	for(int k:gang[index].route[i].items){
		gang[index].route[j].items.push_back(k);
	}
	//Soma os pesos dos nodos
	gang[index].route[j].capacity += gang[index].route[i].capacity;
	//Destroi o nodo i
	gang[index].route.erase(gang[index].route.begin() + i);
}

//Transforma uma rota invalida em valida, juntando nodos repetidos
void fixRoute(int index){
	//Para cada par de nodos, checa se ha repeticao e junta
	for(int i = 0; i < gang[index].route.size(); i++){
		for(int j = i + 1; j < gang[index].route.size(); j++){
			if(gang[index].route[i].id == gang[index].route[j].id ){
				//Se há junção, ajusta os indices
				joinNodes(index, i, j);
				i--;
				j--;
			}
		}
	}
}

/*Construtivo guloso, que gera soluções iniciais com base certos parâmetors*/
void greedyInitialSolution(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0){

	vector<int>actualPos(numThiefs, 0); //Cidade que o ladrão está atualmente
	vector<bool>ended(numThiefs, false); //Indica se o ladrão encerrou a rota

	long double v = (vMax - vMin)/W; //Velocidade v, como definida na descrição do problema

	clearSolution(); //Limpa qualquer solução antiga

	//Salva os melhores resultados atuais
	double bestCost = 0.0;
	vector<Item>bestItems = items;
	vector<Thief>bestGang = gang;
	int bestGangCap = 0;

	bool hasOption = true; //Marca se ainda há opções viáveis (ou que valem a pena)

	while(hasOption){
		
		hasOption = false;

		for(int i = 0; i < numThiefs; i++){
			
			//Se o ladroa voltou termine
			if(ended[i])
				continue;
			
			//Cada ladrao pode fazer n movimentos por iteracao
			int availableMoves = numMoves;

			while(availableMoves--){
				//Inicia o melhor valor com -INFINITO, para permitir melhor exploração
				long double bestValue = numeric_limits<long double>::lowest();
				int bestItem = -1; //Melhor item para por na rota (-1 indica que não existe melhor)

				for(int j = 0; j < M; j++){
					
					//Se o item é muito pesado ou já foi pego, continue
					if(items[j].weight + gangCapacity > W || items[j].thief != -1)
						continue;
					
					//Custos de ir até a cidade do item e de voltar da cidade do item para o início
					long double goingCost = R * adj[actualPos[i]][items[j].city]/(vMax - v * gang[i].capacity);
					long double returnCost = R * adj[items[j].city][0]/(vMax - v * (gang[i].capacity + items[j].weight));
					
					//Se o modo de operação é não seguro, ignoramos o custo de voltar (há possibilidade de negativar, mas explora mais)
					if(!safe) returnCost = 0;

					/*Se o acréscimo daquele item é melhor do que o melhor encontrado, colocamos ele na rota.
					Porém, há uma chance aleatória a de ignorar o item em questão*/
					if(items[j].profit - goingCost - returnCost > bestValue && rand() % 100 >= randomChance){
							bestValue = items[j].profit - goingCost - returnCost;
							bestItem = j;
							hasOption = true; //Houve opção
					}
				}

				//Não existe item que vale a pena para esse ladrão
				if(bestItem == -1){
					actualPos[i] = 0; //Volta ao início
					ended[0] = true; //Encerra
				}
				else{
					//Se não temos cidades na rota ou é a cidade do item não é a última da rota
					if(gang[i].route.empty() || gang[i].route.back().id != items[bestItem].city){
						//Acrescentamos  a cidade
						gang[i].route.push_back(Node());
						gang[i].route.back().id = items[bestItem].city;
						gang[i].route.back().capacity = 0;
					}

					//Somamos o peso do item, acrescentamos na ultima cidade e marcamos o item como pego
					gang[i].route.back().items.push_back(bestItem);
					gang[i].route.back().capacity += items[bestItem].weight;
					gang[i].capacity += items[bestItem].weight;
					gangCapacity += items[bestItem].weight;
					items[bestItem].thief = i;
					actualPos[i] = items[bestItem].city;
				
					//Calcula o valor dessa solucao e atualiza o melhor resultado
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

	//Define a solucao atual como a melhor encontrada
	items = bestItems;
	gang = bestGang;
	gangCapacity = bestGangCap;

	/*Esse construtivo guloso pode gerar soluções invalidas,
	com cidades repetidas em uma mesma rota, por isso deve
	se aplicar a função de conserto nas rotas*/
	for(int i = 0; i < numThiefs; i++)
		fixRoute(i);
}

/*Heuristica GRASP (Greedy Randomized Adaptive Search Procedure), com porcetagem aleatoria fixa
e uma VND no lugar uma busca local comum*/
void GRASP(int numThiefs, bool safe = false, int numMoves = 1, int randomChance = 0, int size = 300, int iter = 500){
	
	start = chrono::system_clock::now(); //Comeca a marcar o tempo

	//Melhores valores encontrados
	vector<Item> bestItem;
	vector<Thief> bestGang;
	int bestGangCap;
	long double bestCost = numeric_limits<long double>::lowest(); //Inicia com -INFINITO

	//Enquanto houveram iterações e tempo
	while(iter-- && !timeLimit(600)){
		//Constroi uma solução gulosa e aleatória
		greedyInitialSolution(numThiefs, safe, numMoves, randomChance);
		
		//Chama a VND
		VND(size);

		//Avalia se melhorou e atualiza os valores
		long double newCost = cost(vMax, vMin, W, R);
		if(newCost > bestCost){
			bestItem = items;
			bestGang = gang;
			bestGangCap = gangCapacity;
			bestCost = newCost;
		}
	}
	
	//Salva a melhor solução encontrada ao final
	items = bestItem ;
	gang = bestGang;
	gangCapacity = bestGangCap;
}

//Remove um item indicado
void removeItem(int id, int cityId, int itemPos){

	//Se não há item para remover
    if(gang[id].route[cityId].items.size() == 0) return;

	//Peso e id do item a ser removido
    int weight = items[gang[id].route[cityId].items[itemPos]].weight;
    int itemId = gang[id].route[cityId].items[itemPos];

	//Remove da rota e atualiza as capacidades
	gang[id].route[cityId].items.erase(gang[id].route[cityId].items.begin() + itemPos);
	gang[id].route[cityId].capacity -= weight;
	gang[id].capacity -= weight;

	//Atualiza a mochila geral e desmarca o item
	gangCapacity -= weight;
	items[itemId].thief = -1;
}

/*Funcao de perturbação da ILS*/
void perturb(double factor) {
        
    //Ajusta o fator para nao ficar muito alto
	if(factor >= 0.4) factor = 0.5;

    for(int i=0; i<gang.size(); i++){
        if(gang[i].route.size() > 1){

			//Numero de items roubados
            int totalStolen = 0;
            for(int k=0; k<gang[i].route.size(); k++) totalStolen += gang[i].route[k].items.size();
            
			//Total de items que deverao restar apos a perturbacao
            totalStolen = (int) ((0.40 + factor) * totalStolen);

			//Remove um item por vez
            while(totalStolen--) {
                
				//Sorteia
				int pos = rand() % (gang[i].route.size() - 1) + 1; // Avoid getting city zero
                
				//Se ainda tem items na cidade além desse
				if(gang[i].route[pos].items.size() > 1) { //Remove só o item
                    int itemId = rand()%gang[i].route[pos].items.size();
                    removeItem(i,pos,itemId);
                }
                else { //Remove o item e a cidade inteira
                   removeItem(i, pos, 0);
                   gang[i].route.erase(gang[i].route.begin() + pos);
                }
            }
        }
    }
}
/*Heuristica ILS (Iterated Local Search), com fator de pertubacao
dinamico e VND com busca local*/
void ILS(int numThiefs, bool safe = false, int numMoves = 1, int size = 100, int iter = 300) {
       
	start = chrono::system_clock::now(); //Comeca a marca o tempo
	
	/*Melhores valores*/
	vector<Item> bestItem;
	vector<Thief> bestGang;
	
	int bestGangCap = 0;
	long double bestCost = numeric_limits<long double>::lowest(); //inicia com -INFINITO
	
	//Gera solução inicial sem aleatório
	greedyInitialSolution(numThiefs, safe, numMoves, 0);
    
	double factor = 0; //Inicia o fator com zero
    int ct = 0; //Iteracoes sem melhoras
    bool improved = false; //Houve melhora?
    
	while(iter-- && !timeLimit(600)) {
        ct++;
        improved = false;

        perturb(factor); //Perturba
        VND(size); //Roda a VND
        
		//Checa se melhorou e atualiza
		long double actualCost = cost(vMax, vMin, W, R);
        if( actualCost  > bestCost ) { //Se melhorou, zero o fator de novo
            improved = true;
            factor = 0;
            ct = 0;
            bestItem = items;
            bestGang = gang;
            bestGangCap = gangCapacity;
            bestCost = actualCost;
        }
        if(ct >= 50 && !improved) { //Se não melhorou apos 50 pertubacoes, aumenta o fator
            factor += 0.05;
        }
    }    

	//Salva a melhor das solucoes
    items = bestItem;
    gang = bestGang;
    gangCapacity = bestGangCap;
}


int main(int argc, char **argv) {
	//Inicia a semente aleatoria
	srand(time(NULL));

	//Le a instancia e os parametros pelo terminal
	readInstance(name, type, V, M, W, vMin, vMax, R);
	char heuristic = argv[1][0];
	int numThiefs = atoi(argv[2]);
	bool safe = atoi(argv[3]);
	int numMoves = atoi(argv[4]);
	int size = atoi(argv[5]);
	int iter = atoi(argv[6]);
	int randomChance = atoi(argv[7]);

	//Ajusta o tamanho do vetor de ladrões
	gang.resize(numThiefs);
	
	//Define a heuristica que sera usada
	if(heuristic == 'g' || heuristic == 'G')
		GRASP(numThiefs, safe, numMoves, randomChance, size, iter);
	else if(heuristic == 'i' || heuristic == 'I')
		ILS(numThiefs, safe, numMoves, size, iter);
	else{
		cerr << "Opcao invalida!!!\n";
		return 1;
	}

	/**Imprime o numero de ladroes, o resultado e o peso coletado na tela**/
	cerr << name << "_" << M << "_" << type << " " << numThiefs << ": " << "\t" << fixed << setprecision(2) << cost(vMax, vMin, W, R) << "\t" << gangCapacity << "\n";
	/****/

	/**Impressao da Solução**/	
	cout << name << "_" << M << "_" << type << ":\n";
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
	cout << endl;
}
