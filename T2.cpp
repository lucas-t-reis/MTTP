#include <bits/stdc++.h>
using namespace std;

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
			ss >> type;
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




	// Copying items

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

}
