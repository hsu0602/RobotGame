#include "iostream"
#include "vector"
#include "ctime"
#include "cstring"
#include "climits"
#include "time.h"
#include "random"
#include "queue"

#define TIME_LIMIT 0.05;
#define SELF 1
#define ENEMY 0

using namespace std;

char identity,enemyIdentity;
int rounds,wide,height;
int maxDepth;

class Clock{
public:
    clock_t start_time;
    clock_t stop_time;

    Clock();
    bool timesUp();
    void totalTime();
};

class Direction{
public:
    int index;
    string serialize;
    int deltaX,deltaY;
    Direction(int index,string serialize,int deltaX,int deltaY);
};

Direction* UP = new Direction(0,"UP",-1,0);
Direction* DOWN = new Direction(1,"DOWN",1,0);
Direction* LEFT = new Direction(2,"LEFT",0,-1);
Direction* RIGHT = new Direction(3,"RIGHT",0,1);
Direction* STAY = new Direction(4,"",0,0);

Direction* directions[4] = {
        UP,DOWN,LEFT,RIGHT
};


class BoardState{
public:
    char** board;
    pair<int,int> locations[2];
    int scores[2];
    int freezes[2];
    int whoMoved = 0;
    Direction* lastDirection = nullptr;
    bool validState = true;
    bool hasConsumed = false;

    BoardState();

    BoardState* move(int who,Direction* direction);
    void init();
    void print();
    int calculateBenefit(int depth);
    bool ended(int depth);
    int calculateDistance();
};

bool isPassAble(char c){
    if(c == 'A' || c == 'B' || c == 'x'){
        return false;
    }
    return true;
}
bool isInBound(int x,int y){
    return 0<=x && x<wide && 0<=y && y<=height;
}

class BestRoute{
public:
    class Node;
    BoardState* rootState;
    Node* rootNode;
    BestRoute(BoardState* rootState);
    vector<vector<Node*> > results;

    void bfs(int depth);
    Direction* getResult(int depth);
    class Node{
    public:
        Direction* startDirection;
        BoardState* boardState;
        Node* parent;
        Node(BoardState* boardState,Node* parent,Direction* startDirection);
        Node* moveTo(Direction* direction);
    };
};

Direction* BestRoute::getResult(int depth) {

    vector<Node*> nodes = results.at(depth);
    int maxScore = 0;
    int tmp[4] = {};

    for (int i=0;i<nodes.size();i++){
        Node* node = nodes.at(i);
        int score= node->boardState->scores[SELF];
        if(score > maxScore){
            memset(tmp,0, sizeof(tmp));
        }
        if(score >= maxScore){
            tmp[nodes.at(i)->startDirection->index] ++;
            maxScore = score;
        }
    }
    int maxIndex = -1000;
    Direction* bestDir = nullptr;
    for(int i=0;i<4;i++){
        if(tmp[i] == maxIndex){
            srand( time(NULL) );
            if(rand()%2){
                bestDir = directions[i];
                maxIndex = tmp[i];
            }
        }
        else if(tmp[i] > maxIndex){
            maxIndex = tmp[i];
            bestDir = directions[i];
        }
    }
    return bestDir;
}

void BestRoute::bfs(int depth){
    vector<Node*> nodes;
    if(depth == 0){
        for (int i = 0; i < 4; ++i) {
            BoardState* boardState = rootState->move(SELF,directions[i]);
            if(boardState != nullptr && boardState->validState){
                Node* node = new Node(boardState,rootNode,directions[i]);
                nodes.push_back(node);
            }
        }
    }
    else{
        vector<Node*> parentNodes = results.at(depth);
        for(int j=0;j<parentNodes.size();j++){
            Node* parentNode = parentNodes.at(j);
            Direction* parentLastDir = parentNode->boardState->lastDirection;
            for(int i=0;i<4;++i){
                BoardState* newState = parentNode->boardState->move(SELF,directions[i]);
                if(newState != nullptr && newState->validState){
                    if(!newState->hasConsumed && directions[i]->deltaX + parentLastDir->deltaX == 0
                    && directions[i]->deltaY + parentLastDir->deltaY == 0){
                       continue;
                    }
                    nodes.push_back(new Node(newState,parentNode,parentNode->startDirection));
                }
            }
        }
    }
    results.push_back(nodes);
}

BestRoute::Node::Node(BoardState *boardState,BestRoute::Node* parent, Direction *startDirection) {
    this->boardState = boardState;
    this->startDirection = startDirection;
    this->parent = parent;
}
BestRoute::BestRoute(BoardState* rootState) {
    this->rootState = rootState;
    this->rootNode = new Node(rootState, nullptr, nullptr);
    vector<Node*> v;
    v.push_back(rootNode);
    this->results.push_back(v);
}

class MiniMax{
public:
    class Node;

    BoardState* initialState;
    Node* root;
    vector<vector<Node*>* >* results;
    MiniMax(BoardState* initialState);

    void bfs(int depth);
    string getResult(int depth);
    void print();

    class Node{
    public:
        int* benefit = nullptr;
        BoardState* boardState;
        Node* parent;
        Direction* lastDirection = nullptr;
        bool shouldKeepSearching = true;
        vector<Node* > children;
        Node(Node* parent,BoardState* boardState,Direction* lastDirection);
        bool isRepeating();
    };
};

bool MiniMax::Node::isRepeating() {
    Node* pNode=this->parent;
    if(pNode != nullptr && !pNode->boardState->hasConsumed){
        Direction* pDirection = pNode->lastDirection;
        Node* ppNode = this->parent;
        if(ppNode != nullptr && !ppNode->boardState->hasConsumed && ppNode->lastDirection == this->lastDirection){
            Node* pppNode = ppNode->parent;
            if(pppNode != nullptr && !pppNode->boardState->hasConsumed && pppNode->lastDirection == pDirection){
                return true;
            }
        }
    }
    return false;
}


class Scanner{
public:
    int enemyDistance;
    bool canReachTreasure;
    bool canReachEnemy;
    double cpMap[4];
    Direction* bestDirection;
    BoardState* boardState;

    Scanner(BoardState* boardState);
    int findEnemy();
};

int Scanner::findEnemy() {
    queue<pair<int,int> > queue;
    int stepMap[20][20] = {};
    for(int i=0;i<wide;i++){
        for(int j=0;j<height;j++){
            stepMap[i][j] = -1;
        }
    }
    pair<int,int> startLoc = boardState->locations[SELF];
    stepMap[startLoc.first][startLoc.second] = 0;
    queue.push(make_pair(startLoc.first,startLoc.second));
    while (!queue.empty()){
        pair<int,int> loc = queue.front();
        queue.pop();
        int step = stepMap[loc.first][loc.second];
        for(int i=0;i<4;i++){
            Direction* direction = directions[i];
            int x = loc.first+direction->deltaX;
            int y = loc.second+direction->deltaY;
            if(!isInBound(x,y)) continue;
            if(stepMap[x][y] != -1) continue;
            char c = boardState->board[x][y];
            if(c == enemyIdentity){
                canReachEnemy = true;
                enemyDistance = step+1;
                return enemyDistance;
            }
            if(isPassAble(c)){
                stepMap[x][y] = step+1;
                queue.push(make_pair(x,y));
            }
        }
    }
    return 1000;
}

int main(){
    Clock clock = Clock();
    srand( time(NULL) );
    cin >> rounds >> wide >> height;
    BoardState* boardState = new BoardState();
    boardState->init();
    maxDepth = 2002 - (rounds*2);
    if(identity == 'B') maxDepth --;
    Scanner scanner(boardState);
    scanner.findEnemy();
    if(scanner.canReachEnemy && scanner.enemyDistance < 10){
        MiniMax* miniMax = new MiniMax(boardState);
        int depth = 0;
        while (!clock.timesUp() && depth < maxDepth){
            miniMax->bfs(depth);
            depth++;
        }
        string result = miniMax->getResult(depth-1);
        cout << result << endl;
        return 0;
    }

    BestRoute* bestRoute = new BestRoute(boardState);
    int depth = 0;
    maxDepth = 1001-rounds;
    if(scanner.canReachEnemy){
        int tmp =(int) (round(scanner.enemyDistance/2));
        if(tmp < maxDepth){
            maxDepth = tmp;
        }
    }
    else if(10 < maxDepth) maxDepth = 10;
    while (!clock.timesUp() && depth < maxDepth){
        bestRoute->bfs(depth);
        depth++;
    }
    Direction* bestDir = bestRoute->getResult(depth-1);
    if(bestDir != nullptr){
        cout << bestDir->serialize << endl;
        return 0;
    }

    Direction* bestDirection = scanner.bestDirection;
    if(bestDirection != nullptr){
        cout << bestDirection->serialize << endl;
    }
    else{
        vector<Direction*> avalibleWay;
        for(int i=0;i<4;i++){
            Direction* item = directions[i];
            int x = boardState->locations[SELF].first + item->deltaX;
            int y = boardState->locations[SELF].second + item->deltaY;
            if(isInBound(x,y) && boardState->board[x][y] == '.'){
                avalibleWay.push_back(item);
            }
        }
        srand( time(NULL) );
        cout << avalibleWay.at((rand()%avalibleWay.size()))->serialize << endl;
    }
}

MiniMax::MiniMax(BoardState *initialState) {
    this->results = new vector<vector<Node*>* >;
    this->initialState = initialState;
    root = new Node(nullptr,initialState, nullptr);
    vector<Node*>* rootVector = new vector<Node*>;
    rootVector->push_back(root);
    results->push_back(rootVector);
}

MiniMax::Node::Node(Node *parent, BoardState *boardState,Direction* lastDirection) {
    this->parent = parent;
    this->boardState = boardState;
    this->lastDirection = lastDirection;
}


Scanner::Scanner(BoardState* boardState) {
    enemyDistance = 1000;
    this->boardState = boardState;
    canReachTreasure = false;
    canReachEnemy = false;
    memset(cpMap,0,sizeof (cpMap));
    bestDirection = nullptr;

    int startX = boardState->locations[SELF].first;
    int startY = boardState->locations[SELF].second;
    double directionScore[4] = {};
    for(int i=0;i<4;i++){
        int nX = startX + directions[i]->deltaX;
        int nY = startY + directions[i]->deltaY;

        if(!isInBound(nX,nY)) continue;
        char c = boardState->board[nX][nY];
        if(!isPassAble(boardState->board[nX][nY])) continue;
        if(c != 'm' && c != 's' && c!= '.' && c != 'b') continue;
        if(c == 'm'){
            cpMap[i] += 1;
        }
        else if(c == 's') cpMap[i] += boardState->scores[SELF];

        int steps[20][20] = {};
        queue<pair<int,int> > queue;
        steps[startX][startY] = -100;

        steps[nX][nY] = 1;
        queue.push(make_pair(nX,nY));
        while (!queue.empty()){
            pair<int,int> loc = queue.front();
            int step = steps[loc.first][loc.second] + 1;
            for (int m=0;m<4;m++){
                Direction* direction = directions[m];
                int x = loc.first + direction->deltaX;
                int y = loc.second + direction->deltaY;
                if(steps[x][y] != 0) continue;
                if(!isInBound(x,y)) continue;
                char c = boardState->board[x][y];
                steps[x][y] = step;
                if(c != 'm' && c != 's' && c!= '.' && c != 'b') continue;
                queue.push(make_pair(x,y));
                if(c == 'm'){
                    cpMap[i] += 1.0/(step);
                }
                else if(c == 's'){
                    cpMap[i] += (boardState->scores[SELF])/step;
                }
            }
            queue.pop();
        }
    }
    double bestCP = 0;
    for(int i=0;i<4;i++){
        if(cpMap[i] > bestCP){
            bestCP = cpMap[i];
            bestDirection = directions[i];
        }
    }
    if(bestCP > 0) canReachTreasure = true;
}

Direction::Direction(int index,string serialize, int deltaX, int deltaY) {
    this->deltaX = deltaX;
    this->index = index;
    this->deltaY = deltaY;
    this->serialize = serialize;
}

BoardState::BoardState() {
    board = new char*[wide];
    for(int i=0;i<wide;++i){
        board[i] = new char [height];
    }
}

void BoardState::init() {
    int aScore,bScore;
    pair<int,int> aLoc;
    pair<int,int> bLoc;
    for(int i=0;i<wide;i++){
        for(int j=0;j<height;j++){
            cin >> board[i][j];
            if(board[i][j] == 'A'){
                aLoc = make_pair(i,j);
            }
            else if(board[i][j] == 'B'){
                bLoc = make_pair(i,j);
            }
        }
    }
    cin >> aScore >> bScore >> identity;
    freezes[0] = 0;
    freezes[1] = 0;
    if(identity == 'A'){
        enemyIdentity = 'B';
        locations[SELF] = aLoc;
        locations[ENEMY] = bLoc;
        scores[SELF] = aScore;
        scores[ENEMY] = bScore;
    }
    else {
        scores[ENEMY] = aScore;
        scores[SELF] = bScore;
        locations[ENEMY] = aLoc;
        locations[SELF] = bLoc;
        enemyIdentity = 'A';
    }

}

bool BoardState::ended(int depth) {
    if(depth == maxDepth) return true;
    int benefit = calculateBenefit(depth);
    if(benefit == INT_MAX || benefit == INT_MIN) return true;
    bool foundElement = false;
    for(int i=0;i<wide;i++){
        for(int j=0;j<height;j++){
            char c = board[i][j];
            switch (c) {
                case 'b':
                case 'm':
                case 'n':
                case 's':
                case 't':
                    foundElement = true;
                    break;
            }
        }
    }
    return !foundElement;
}

void MiniMax::bfs(int depth) {
    vector<Node*>* parents = results->at(depth);
    vector<Node*>* newNodes = new vector<Node*>;
    results->push_back(newNodes);

    for (int j=0;j<parents->size();j++){
        Node* parentNode = parents->at(j);
        if(!parentNode->shouldKeepSearching) continue;
        for (int i=0;i<4;i++){
            Direction* direction = directions[i];
            BoardState* parentState = parentNode->boardState;
            BoardState* newState = parentState->move(!(parentState->whoMoved),direction);
            if(newState != nullptr && newState->validState){
                Node* node = new Node(parentNode,newState,direction);
                if(newState->ended(depth+1) || node->isRepeating()){
                    node->shouldKeepSearching = false;
                }

                newNodes->push_back(node);
                parentNode->children.push_back(node);
            }
        }
    }
}

string MiniMax::getResult(int depth) {
    while (depth >= 0){
        vector<Node*> * parentNodes = results->at(depth);
        for (int i=0;i<parentNodes->size();++i){
            Node* parentNode = parentNodes->at(i);
            bool findMax = !(parentNode->boardState->whoMoved);
            int* tmp = findMax ? new int(INT_MIN) : new int(INT_MAX);
            for (int j=0;j<parentNode->children.size();++j){
                Node* childNode = parentNode->children.at(j);
                int* childBenefit = childNode->benefit;
                if(childBenefit == nullptr) {
                    childBenefit = new int(childNode->boardState->calculateBenefit(depth + 1));
                    childNode->benefit = childBenefit;
                }
                if(findMax){
                    if(*tmp < *childBenefit) tmp = childBenefit;
                }
                else if(*tmp > *childBenefit) tmp = childBenefit;
            }
            if(*tmp == INT_MAX || *tmp == INT_MIN) parentNode->benefit = new int (parentNode->boardState->calculateBenefit(depth));
            else parentNode->benefit = tmp;
        }
        depth--;
    }
    int max = INT_MIN;
    Direction* bestDir;
    for(int i=0;i<root->children.size();i++){
        Node* item = root->children.at(i);
        if(*(item->benefit) == max){
            srand( time(NULL) );
            if(rand()%2){
                bestDir = item->lastDirection;
            }
        }
        if(*(item->benefit) > max){
            bestDir = item->lastDirection;
            max = *(item->benefit);
        }
    }
    root->benefit = &max;

    return bestDir->serialize;
}


BoardState *BoardState::move(int who,Direction* direction) {
    pair<int,int> location = this->locations[who];
    int score = this->scores[who];
    int freeze = this->freezes[who];
    bool isNewBoardValid = true;
    if(freeze > 0){
        freeze --;
    }
    else{
        location.first += direction->deltaX;
        location.second += direction->deltaY;
        if( 0<=location.first && location.first<wide &&
            0<=location.second && location.second<wide){
            char element = board[location.first][location.second];
            switch (element) {
                case 'A':
                case 'B':
                case 'x':
                    isNewBoardValid = false;
                    break;
                case 'm':
                    hasConsumed = true;
                    score += 1; break;
                case 'n':
                    hasConsumed = true;
                    score -= 1; break;
                case 's':
                    hasConsumed = true;
                    score *= 2; break;
                case 't':
                    hasConsumed = true;
                    score /= 2; break;
                case 'b':
                    hasConsumed = true;
                    freeze = 3; break;
            }
        }
        else{
            isNewBoardValid = false;
        }
    }
    if(isNewBoardValid){
        BoardState* newState = new BoardState();
        newState->whoMoved = who;
        newState->lastDirection = direction;
        for(int i=0;i<wide;i++){
            for(int j=0;j<height;j++){
                newState->board[i][j] = this->board[i][j];
            }
        }
        newState->board[locations[who].first][locations[who].second] = '.';
        newState->board[location.first][location.second] = (who) ? identity : enemyIdentity;
        newState->locations[who] = location;
        newState->scores[who] = score;
        newState->freezes[who] = freeze;
        newState->scores[!who] = this->scores[!who];
        newState->locations[!who] = this->locations[!who];
        newState->freezes[!who] = this->freezes[!who];
        return newState;
    }
    return nullptr;
}


void MiniMax::print() {

}

bool testLocation(char** board,pair<int,int> location,int depth){
    if(depth == maxDepth) return true;
    int goodLocation = false;
    for(int i=0;i<4;i++){
        Direction* direction = directions[i];
        int x = location.first + direction->deltaX;
        int y = location.second + direction->deltaY;
        if(0<=x && x<wide && 0<=y && y<wide){
            char c = board[x][y];
            if(c != 'A' && c != 'B' && c != 'x'){
                goodLocation = true;
                break;
            }
        }
    }
    return goodLocation;
}

int BoardState::calculateBenefit(int depth) {
    if(!testLocation(board,locations[SELF],depth)){
        return -2100000000;
    }
    if(!testLocation(board,locations[ENEMY],depth)){
        return 2100000000;
    }
    if(depth == maxDepth){

    }
    return scores[SELF] - scores[ENEMY];
}


void BoardState::print() {
    for (int i=0;i<wide;i++) {
        for(int j=0;j<height;j++){
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Your Score: "<< scores[1] << endl;
    cout << "Enemy Score "<< scores[0] << endl;
    cout << endl;
}

Clock::Clock() {
    start_time = clock();
}

bool Clock::timesUp() {
    stop_time = clock();
    return ((double(stop_time) - double(start_time)) / CLOCKS_PER_SEC) > TIME_LIMIT;
}
void Clock::totalTime() {
    cout << ((double(stop_time) - double(start_time)) / CLOCKS_PER_SEC) <<endl;
}
