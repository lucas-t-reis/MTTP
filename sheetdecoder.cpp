#include<bits/stdc++.h>

using namespace std;

/*Important info: it decodes sheets,
not shits;*/

int main(){
    long double shit, sheet;
    int cont = 0;

    for(int i = 1; i <= 5; i++)
        for(int j = 10; j <= 25; j += 5)
            for(int k = 0; k <= 1; k++)
                cout << k << "--" << j << "--" << i << "\t";
    cout << "\n";

    while(cin >> shit >> shit >> shit >> sheet){
        cout << sheet << "\t";
        cont++;
        if(cont == 40){
            cout << "\n";
            cont = 0;
        }
    }
}
