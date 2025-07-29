#include <iostream>
#include <cstdlib>
using namespace std;

void process(int round, int n_row, int n_col, int score_A, int score_B, char role, char** table);

int main(){
    int round, n_row, n_col, score_A, score_B;
    char role;
    char** table;

    scanf("%d\n", &round);
    scanf("%d %d\n", &n_row, &n_col);

    table = (char**) malloc(n_row * sizeof(char*));

    for(int i = 0 ; i < n_row ; i++){
        table[i] = (char*) malloc(n_col * sizeof(char));
    }

    for(int i = 0 ; i < n_row ; i++){
        for(int j = 0 ; j < n_col ; j++){
            scanf("%c%*c", &table[i][j]);
        }
    }

    scanf("%d\n", &score_A);
    scanf("%d\n", &score_B);
    scanf("%c", &role);

    process(round, n_row, n_col, score_A, score_B, role, table);

    return 0;
}

void process(int round, int n_row, int n_col, int score_A, int score_B, char role, char** table){
    if(role == 'A'){
        srand(11916 + round);

        int ax, ay;

        for(int i = 0 ; i < n_row ; i++){
            for(int j = 0 ; j < n_col ; j++){
                if(table[i][j] == 'A'){
                    ay = i;
                    ax = j;
                }
            }
        }

        string str[4] = {"UP", "DOWN", "LEFT", "RIGHT"};

        int turn;

        while(1){
            int r = rand() % 10;

            if(r < 3){
                turn = 0;

                if((ay != 0) && (table[ay - 1][ax] != 'x') && (table[ay - 1][ax] != 'B')){
                    break;
                }
            }
            else if(r < 5){
                turn = 1;

                if((ay != n_row - 1) && (table[ay + 1][ax] != 'x') && (table[ay + 1][ax] != 'B')){
                    break;
                }
            }
            else if(r < 8){
                turn = 2;

                if((ax != 0) && (table[ay][ax - 1] != 'x') && (table[ay][ax - 1] != 'B')){
                    break;
                }
            }
            else{
                turn = 3;

                if((ax != n_col - 1) && (table[ay][ax + 1] != 'x') && (table[ay][ax + 1] != 'B')){
                    break;
                }
            }
        }

        cout << str[turn] << endl;
    }
    else{
        if(round < 514){
            int ax, ay, bx, by;

            for(int i = 0 ; i < n_row ; i++){
                for(int j = 0 ; j < n_col ; j++){
                    if(table[i][j] == 'A'){
                        ay = i;
                        ax = j;
                    }
                    else if(table[i][j] == 'B'){
                        by = i;
                        bx = j;
                    }
                }
            }

            if(bx * (-1) + n_col - 1 == ax){
                if(by * (-1) + n_row - 1 == ay + 1){
                    cout << "DOWN" << endl;
                }
                else{
                    cout << "UP" << endl;
                }
            }
            else{
                if(bx * (-1) + n_col - 1 == ax + 1){
                    cout << "RIGHT" << endl;
                }
                else{
                    cout << "LEFT" << endl;
                }
            }
        }
        else{
            srand(11916 + round);

            int ax, ay;

            for(int i = 0 ; i < n_row ; i++){
                for(int j = 0 ; j < n_col ; j++){
                    if(table[i][j] == 'B'){
                        ay = i;
                        ax = j;
                    }
                }
            }

            string str[4] = {"UP", "DOWN", "LEFT", "RIGHT"};

            int turn;

            while(1){
                int r = rand() % 10;

                if(r < 3){
                    turn = 0;

                    if((ay != 0) && (table[ay - 1][ax] != 'x') && (table[ay - 1][ax] != 'A')){
                        break;
                    }
                }
                else if(r < 5){
                    turn = 1;

                    if((ay != n_row - 1) && (table[ay + 1][ax] != 'x') && (table[ay + 1][ax] != 'A')){
                        break;
                    }
                }
                else if(r < 8){
                    turn = 2;

                    if((ax != 0) && (table[ay][ax - 1] != 'x') && (table[ay][ax - 1] != 'A')){
                        break;
                    }
                }
                else{
                    turn = 3;

                    if((ax != n_col - 1) && (table[ay][ax + 1] != 'x') && (table[ay][ax + 1] != 'A')){
                        break;
                    }
                }
            }

            cout << str[turn] << endl;
        }
    }
}

