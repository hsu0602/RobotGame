#include <iostream>
#include <cstdlib>
#include <utility>
#include <string>

using namespace std;
typedef pair<int, int> PII;

#define F first
#define S second
#define mkp make_pair

int Round, M, N;
char Graph[20][20]={};

bool valid(int x, int y){
    return (
        x >= 0 && x < M && y >=0 && y < N && 
        Graph[x][y] != 'x' && 
        Graph[x][y] != 'A' && Graph[x][y] != 'B'
    );
}

string next_dir(int x, int y, int num){
    for(int i=0;i<4;i++){
        switch(num%4) {
            case 0: 
                if(valid(x-1, y))
                    return "UP";
                else break;
            case 1:
                if(valid(x+1, y)) 
                    return "DOWN";
                else break;
            case 2: 
                if(valid(x, y-1))
                    return "LEFT";
                else break;
            case 3: 
                if(valid(x, y+1))
                    return "RIGHT";
                else break;
            default: return "RIGHT";
        }
        num++;
    }
    return "RIGHT";
}

int main(void){
    int A, B;
    char me;
    cin >> Round >> M >> N;
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
            cin >> Graph[i][j];
    cin >> A >> B >> me;
    int x, y, rand_seed;
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
            if(Graph[i][j] == me){
                x = i;
                y = j;
                rand_seed = i + j;
            }
    srand(rand_seed);
    cout << next_dir(x, y, rand()) << endl;
}