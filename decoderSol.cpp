#include<iostream>
#include<fstream>

using namespace std;

int main(){
    string line, comp = "_03";
    int cont = 1;
    ofstream out;
    while(getline(cin, line)){
        if(isalpha(line[0])){
            cont++;
            if(cont == 2 || cont == 7){
                out.close();
                line.pop_back();
                out.open(line + comp + ".sol");
            }
            if(cont == 6){
                comp = "_07";
            }
            else if(cont == 11){
                comp = "_03";
                cont = 1;
            }
        }
        else
            out << line << "\n";
    }
}