#include<bits/stdc++.h>
using namespace std;

// coordinates
struct coord{
    int x;
    int y;
};

// inputs
char mp[20][20];
int r, m, n;
int A_score, B_score;
char me;

//instructions and directions
string instructions[4] = {"UP", "DOWN", "LEFT", "RIGHT"};
int dirx[4] = { -1, 1,  0, 0};
int diry[4] = {  0, 0, -1, 1};

void get_input(){

    cin >> r;
    cin >> m >> n;

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            cin >> mp[i][j];
        }
    }

    cin >> A_score >> B_score >> me;

    return; // void function, no return entity
}

coord get_my_pos(){

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            if(mp[i][j] == me)
                return {i, j};
        }
    }

    return {0, 0}; // return coord(0, 0) if unexpected

}


bool move_is_valid(coord pos, string ins){

    for(int i = 0; i < 4; i++){
        if(instructions[i] == ins){
            return (
                pos.x + dirx[i] >= 0 && pos.x + dirx[i]  < m &&
                pos.y + diry[i] >= 0 && pos.y + diry[i] < n
            );
        }
    }
    return false; // return false if "ins" unexpected
}


string pick_instructions(){

    // randint in [a, b]
    srand(time(NULL));
    int a = 0, b = 3;
    int r = rand() % (b - a + 1) + a;

    return instructions[r];

}

/*int get_millis_now(){
    auto start = chrono::system_clock::now();
    auto duration = start.time_since_epoch();
    return (int)chrono::duration_cast<chrono::milliseconds>(duration).count();
}
void timer(){

    static int mode = 0;
    static int start_time;
    static int end_time;

    if(mode == 0){
        start_time =  get_millis_now();
        mode  = 1;
    }else{
        end_time =  get_millis_now();
        cerr << "[Info] Elapsed Time : " << end_time - start_time << " ms" << endl;
        mode = 0;
    }
    return;
}*/


int main(){

    get_input();

    //timer(); //start_timer

    coord my_pos = get_my_pos();

    // random pick one
    string ins;
    do{
        ins = pick_instructions();
    }while(!move_is_valid(my_pos, ins));
    cout << ins << endl;

    //timer(); //end_timer

    return 0;
}
