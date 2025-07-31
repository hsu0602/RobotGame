#include <bits/stdc++.h>
using namespace std;

const int MAX = 20;

struct Pos {
    int x, y;
    bool operator==(const Pos &o) const { return x == o.x && y == o.y; }
};

char grid[MAX][MAX];
int m, n, round_num;
int Ascore, Bscore;
char player;
Pos myPos;

string directions[4] = {"UP", "DOWN", "LEFT", "RIGHT"};
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};

// 判斷是否在地圖範圍內
bool in_grid(int x, int y) {
    return x >= 0 && x < m && y >= 0 && y < n;
}

// 評估地圖格子分數(路徑搜尋用)
int eval_tile(char c, int cur_score) {
    switch(c) {
        case 'm': return 10;           // 蘑菇 +1分(吸引走向)
        case 'n': return -10;          // 毒蘑菇 -1分(避開)
        case 's': return cur_score;    // 無敵星星 *2效果評分
        case 't': return -(cur_score / 2); // 無敵毒星星 /2效果評分
        case 'b': return -100;         // 地雷負分，BFS避免踩
        default: return 0;
    }
}

// 找玩家位置
void find_my_position() {
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            if (grid[i][j] == player || grid[i][j] == 'X') {
                myPos = {i, j};
                return;
            }
        }
    }
}

// BFS尋找最近且分數最高寶物，回傳第一步方向索引，找不到回-1
int bfs_best_target() {
    queue<Pos> q;
    bool visited[MAX][MAX] = {};
    int dist[MAX][MAX];
    Pos parent[MAX][MAX];

    for (int i=0; i<m; i++)
        for (int j=0; j<n; j++)
            dist[i][j] = INT_MAX;

    q.push(myPos);
    visited[myPos.x][myPos.y] = true;
    dist[myPos.x][myPos.y] = 0;

    vector<Pos> candidates;
    int best_score = INT_MIN;

    while (!q.empty()) {
        Pos cur = q.front(); q.pop();

        char c = grid[cur.x][cur.y];
        int val = eval_tile(c, (player == 'A') ? Ascore : Bscore);

        if (val > 0 && !(cur == myPos)) { // 找到目標寶物
            if (val > best_score) {
                candidates.clear();
                candidates.push_back(cur);
                best_score = val;
            } else if (val == best_score) {
                candidates.push_back(cur);
            }
        }

        for (int d=0; d<4; d++) {
            int nx = cur.x + dx[d], ny = cur.y + dy[d];
            if (!in_grid(nx, ny)) continue;
            if (visited[nx][ny]) continue;
            if (grid[nx][ny] == 'b') continue; // BFS避免地雷
            visited[nx][ny] = true;
            dist[nx][ny] = dist[cur.x][cur.y] + 1;
            parent[nx][ny] = cur;
            q.push({nx, ny});
        }
    }

    if (candidates.empty()) return -1;

    Pos target = candidates[0];
    int min_dist = dist[target.x][target.y];
    for (auto &c : candidates) {
        if (dist[c.x][c.y] < min_dist) {
            target = c;
            min_dist = dist[c.x][c.y];
        }
    }

    Pos cur = target;
    while (!(parent[cur.x][cur.y] == myPos)) {
        cur = parent[cur.x][cur.y];
    }

    for (int d=0; d<4; d++) {
        if (myPos.x + dx[d] == cur.x && myPos.y + dy[d] == cur.y) {
            return d;
        }
    }
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> round_num >> m >> n;
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            cin >> grid[i][j];
        }
    }
    cin >> Ascore >> Bscore >> player;

    find_my_position();

    int d = bfs_best_target();

    if (d == -1) {
        // 找不到寶物，隨機走合法方向
        vector<int> legal;
        for (int i=0; i<4; i++) {
            int nx = myPos.x + dx[i], ny = myPos.y + dy[i];
            if (in_grid(nx, ny)) legal.push_back(i);
        }
        if (legal.empty()) d = 0;
        else d = legal[rand() % legal.size()];
    }

    cout << directions[d] << "\n";

    return 0;
}