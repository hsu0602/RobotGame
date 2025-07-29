#include <iostream>
#include <map>
#include <queue>
using namespace std;

enum Direction { UP,
                 LEFT,
                 RIGHT,
                 DOWN };

string defaultOutput(Direction dir) {
    switch (dir) {
        case UP:
            return "UP";
        case DOWN:
            return "DOWN";
        case LEFT:
            return "LEFT";
        case RIGHT:
            return "RIGHT";
    }
    return "FALSE";
}

class Position {
   public:
    Position(int _score, Direction _direction, pair<int, int> _position) {
        score = _score;
        direction = _direction;
        position = _position;
    }
    void print() {
        cout << "==========" << endl;
        cout << "score: " << score << endl;
        cout << "direction: " << defaultOutput(direction) << endl;
        cout << "position: (" << position.first << ", " << position.second << ")" << endl;
        cout << "==========" << endl;
    }
    int score;
    Direction direction;
    pair<int, int> position;
};

class Timer {
   private:
    clock_t start_time;
    clock_t stop_time;

   public:
    void startTimer() {
        start_time = clock();
    }
    void stopTimer() {
        stop_time = clock();
    }
    double getTime() {
        return (double(stop_time) - double(start_time)) / CLOCKS_PER_SEC;
    }
};

class Random {
   public:
    int generateRandom(int from, int to) {
        int range = to - from + 1;
        return (rand() % range) + from;
    }
    void setRandom() {
        srand(48763);
    }
};

class BOT : public Timer, public Random {
   public:
    BOT(int round, int width, int height);
    ~BOT();
    void init();
    string makeDecision();

   private:
    bool validatePosition(int x, int y);
    pair<int, int> getCurrentPosition();
    int calculateScore(int currentScore, int x, int y);
    char** board;
    int round;
    int width, height;
    int AScore, BScore;
    pair<int, int> ALocation, BLocation;
    char identity;
};

BOT::BOT(int _round, int _width, int _height) {
    round = _round;
    width = _width;
    height = _height;
}

BOT::~BOT() {
    for (int i = 0; i < height; i++) {
        delete[] board[i];
    }
    delete[] board;
}

void BOT::init() {
    board = new char*[height];
    for (int i = 0; i < height; i++) {
        board[i] = new char[width];
        for (int j = 0; j < width; j++) {
            cin >> board[i][j];
            if (board[i][j] == 'A') ALocation = make_pair(j, i);
            if (board[i][j] == 'B') BLocation = make_pair(j, i);
        }
    }
    cin >> AScore >> BScore >> identity;
}

pair<int, int> BOT::getCurrentPosition() {
    if (identity == 'A')
        return ALocation;
    else if (identity == 'B')
        return BLocation;
    return make_pair(-1, -1);
}

bool BOT::validatePosition(int x, int y) {
    if (x < 0 || x >= width)
        return false;
    else if (y < 0 || y >= height)
        return false;
    else if (board[y][x] == 'A' || board[y][x] == 'B')
        return false;
    else if (board[y][x] == 'x')
        return false;
    return true;
}

int BOT::calculateScore(int currentScore, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return -1;
    char gettingObject = board[y][x];
    switch (gettingObject) {
        case 'm':
            return currentScore + 1;
        case 'n':
            return currentScore - 1;
        case 's':
            return currentScore * 2;
        case 't':
            return currentScore / 2;
        case '.':
        case 'b':
        default:
            return currentScore;
    }
}

string BOT::makeDecision() {
    bool** visited = new bool*[height];
    for (int i = 0; i < height; i++) {
        visited[i] = new bool[width];
        for (int j = 0; j < width; j++) {
            visited[i][j] = false;
        }
    }

    int currentMaxScore = 0;
    Direction currentDirection;

    queue<Position> q;
    pair<int, int> currentPosition = getCurrentPosition();
    Position initPsitions[4] = {
        Position(0, LEFT, make_pair(currentPosition.first - 1, currentPosition.second)),
        Position(0, RIGHT, make_pair(currentPosition.first + 1, currentPosition.second)),
        Position(0, UP, make_pair(currentPosition.first, currentPosition.second - 1)),
        Position(0, DOWN, make_pair(currentPosition.first, currentPosition.second + 1))};
    map<int, int> refrenceOrder;
    setRandom();
    for (int i = 0; i < 4; i++) {
        refrenceOrder[generateRandom(0, INT32_MAX)] = i;
    }
    for (const auto& s : refrenceOrder) {
        int i = s.second;
        if (validatePosition(initPsitions[i].position.first, initPsitions[i].position.second)) {
            // Tp prevent no legal steps.
            currentDirection = initPsitions[i].direction;
            q.push(initPsitions[i]);
        } else {
            initPsitions[i].score = -1;
        }
    }
    refrenceOrder.clear();
    visited[currentPosition.second][currentPosition.first] = true;
    while (!q.empty()) {
        if (getTime() > 0.9) break;
        queue<Position> tmpq;
        while (!q.empty()) {
            if (getTime() > 0.9) break;
            Position tmp = q.front();
            Position tmpInitPsitions[4] = {
                Position(calculateScore(tmp.score, tmp.position.second, tmp.position.first - 1), tmp.direction, make_pair(tmp.position.first - 1, tmp.position.second)),
                Position(calculateScore(tmp.score, tmp.position.second, tmp.position.first + 1), tmp.direction, make_pair(tmp.position.first + 1, tmp.position.second)),
                Position(calculateScore(tmp.score, tmp.position.second - 1, tmp.position.first), tmp.direction, make_pair(tmp.position.first, tmp.position.second - 1)),
                Position(calculateScore(tmp.score, tmp.position.second + 1, tmp.position.first), tmp.direction, make_pair(tmp.position.first, tmp.position.second + 1))};
            setRandom();
            for (int i = 0; i < 4; i++) {
                refrenceOrder[generateRandom(0, INT32_MAX)] = i;
            }
            for (const auto& s : refrenceOrder) {
                int i = s.second;
                if (validatePosition(tmpInitPsitions[i].position.first, tmpInitPsitions[i].position.second) && !visited[tmp.position.second][tmp.position.first]) {
                    if (tmpInitPsitions[i].score > currentMaxScore) {
                        currentMaxScore = tmpInitPsitions[i].score;
                        currentDirection = tmpInitPsitions[i].direction;
                    }
                    tmpq.push(tmpInitPsitions[i]);
                }
            }
            visited[tmp.position.second][tmp.position.first] = true;
            q.pop();
            refrenceOrder.clear();
        }
        q = tmpq;
    }
    delete[] visited;
    return defaultOutput(currentDirection);
}

int main(void) {
    int round, M, N;
    cin >> round >> M >> N;
    BOT bot(round, N, M);
    bot.startTimer();
    bot.init();
    cout << bot.makeDecision() << endl;
    bot.stopTimer();
}
