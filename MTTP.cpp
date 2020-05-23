#include "core.h" 			// Contains the problem modeling, auxiliar functions, libraries and global variables
#include "mheuristics.h"	// GRASP and ILS implementations

using namespace std;

int main(int argc, char **argv) {

	srand(time(NULL));
	
	// Reading problem data and Input parameters
	readInstance(name, type, V, M, W, vMin, vMax, R);

	char heuristic = 	argv[1][0];
	int numThiefs = 	atoi(argv[2]);
	bool safe = 		atoi(argv[3]);
	int numMoves = 		atoi(argv[4]);
	int size = 			atoi(argv[5]);
	int iter = 			atoi(argv[6]);
	int randomChance = 	atoi(argv[7]);

	gang.resize(numThiefs);
	
	if(heuristic == 'g' || heuristic == 'G')
		GRASP(numThiefs, safe, numMoves, randomChance, size, iter);
	else if(heuristic == 'i' || heuristic == 'I')
		ILS(numThiefs, safe, numMoves, size, iter);
	else {
		cerr << "Invalid option. You provided " << heuristic << " and the valid options are g/G or i/I\n";
		exit(EXIT_FAILURE);
	}

	printSolution();
	
}
