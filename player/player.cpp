#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>

int step, row, col, sx, sy, range_x, range_y, k, front, rear;
int maxi = 0;;
char me;
int a_x, a_y, b_x, b_y, ox, oy; 
float score = 0, max = 0;
int score_a, score_b = 0;
int dx[] = {0, 1, 0, -1 }, dy[] = {1, 0, -1, 0};
char opp = 'B';
int pos = 0;
float points[4] = {};
char graph[21][21] = {};
float map[21][21] = {};
const char *dir[] = {"RIGHT", "DOWN", "LEFT", "UP"};
struct Queue{
    int x;
    int y;
};
struct Queue queue[21*21] = {0};
void en( int x1, int y1 ) {
    queue[rear].x = x1;
    queue[rear].y = y1;
    rear++;
}
void de(){
    front++;
}
struct Queue get(){
    return queue[front];
}
void act( int a, int b ){
    if( graph[a][b] == '.' ) {
        score += 0;
    }
    if( graph[a][b] == 'm' ) {
        score += 1/map[a][b];
    }
    if( graph[a][b] == 's' ) {
        score += score/map[a][b]; 
    }
    if( graph[a][b] == 'n' ) {
        score -= 1/map[a][b];
    }
    if( graph[a][b] == 't' ) {
        score -= score/map[a][b];
    } 
    if( graph[a][b] == 'b' ){
       map[a][b] += 3;
       score += 0;
    }
}
void clean(){
     memset( map, 0, sizeof(map[0][0])*21*21);
     front = 0;
     rear = 0;
     score = 0;
}
bool Val( int x, int y ) {
    if( 0 <= x && x < col && 0 <= y && y < row) {
      if( graph[x][y] != me && graph[x][y] != 'x') {
          return true;
      } else {
          return false;
      }
  } 
  return false;
}
void decide(){
    for( int index = 0; index < 4; index++) {
        int px = sx + dx[index];
        int py = sy + dy[index];
        if( Val( px, py ) && graph[ px ][ py ] != opp){
                if( max == 0 ) {
                    max = points[index];
                    maxi = index;
                } else  {
                    if( points[index] >= max ) {
                    max = points[index];
                    maxi = index;
                    }
                }
            }
         }
}
bool empty(){
    if(front == rear)
        return true;
    return false;
}
void bfs( int x, int y ) {
    while( !empty() ) {
        struct Queue cur_q = get();
        de();
        for ( k = 0; k < 4; k++) {
            int cur_x = cur_q.x + dx[k]; 
            int cur_y = cur_q.y + dy[k];
               if( Val( cur_x, cur_y ) && graph[cur_x][cur_y] != opp && map[cur_x][cur_y] == 0 ) {
                   en( cur_x, cur_y );
                    map[cur_x][cur_y] = map[cur_q.x][cur_q.y] + 1;
                    if( map[cur_x][cur_y] == 16 )
                        return;
                        act( cur_x, cur_y );
                }
         }
     }
}
int main(){
    scanf("%d\n", &step); 
    scanf("%d %d\n", &row, &col);
        for (int i = 0;i < row; i++) {
             for (int j = 0; j < col; j++) {
                if( j == 0 ) {
                    scanf("\n%c", &graph[i][j]);
                } else {
                    scanf(" %c", &graph[i][j]);
                }
                if( graph[i][j] == 'A' ) {
                    a_x = i, a_y = j;
                    pos = 1;
                 }
                if( graph[ i ][ j ] == 'B' ){
                    b_x = i, b_y = j;
                }
            }
        }
    scanf("%d\n", &score_a);
    scanf("%d\n", &score_b);
    scanf("%c", &me);
    if ( me == 'A' ){
        sx = a_x, sy = a_y;
    } else {
        sx = b_x, sy = b_y;
        opp = 'A';
      int tmp = score_a;
       score_a = score_b;
       score_b = tmp;
    }
        for(int k = 0; k < 4; k++){
            clean();
            int now_x = sx + dx[k]; 
            int now_y = sy + dy[k];
               if( Val(now_x, now_y) && graph[now_x][now_y] != opp && graph[now_x][now_y] != 't' && map[now_x][now_y] == 0 ) {
                   en(now_x, now_y);
                   map[now_x][now_y] = 1;
                   if( map[now_x][now_y] == 16)
                       break;
                    act(now_x, now_y);
                 bfs(sx, sy); 
             }
             points[k]  = score;
        }
    decide(); 
    printf("%s\n", dir[maxi]); 
}