#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
using namespace std;

// 定义地图大小
const int ROW = 10;
const int COL = 12;

// 定义符号
const char WALL = '#';
const char EMPTY = '.';
const char MAN = '@';
const char BOX = '$';
const char GOAL = '*';
const char MAN_ON_GOAL = '+';
const char BOX_ON_GOAL = '!';

// 定义方向
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

// 定义方向数组
const int dx[4] = { -1, 1, 0, 0 };
const int dy[4] = { 0, 0, -1, 1 };

// 定义操作数组
const char op[4] = { 'U', 'D', 'L', 'R' };

// 定义状态结构体
struct State {
    char map[ROW][COL]; // 地图数组
    int man_x; // 人的横坐标
    int man_y; // 人的纵坐标
    State* prev; // 前驱状态指针
    char action; // 前一步操作

    // 构造函数
    State(char m[ROW][COL], int x, int y, State* p, char a) {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                map[i][j] = m[i][j];
            }
        }
        man_x = x;
        man_y = y;
        prev = p;
        action = a;
    }

    // 判断是否是目标状态，即所有箱子都在目标点上
    bool isGoal() {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (map[i][j] == BOX || map[i][j] == MAN) {
                    return false;
                }
            }
        }
        return true;
    }

    // 判断是否可以向某个方向移动或推动箱子
    bool canMove(int dir) {
        int nx = man_x + dx[dir];
        int ny = man_y + dy[dir];
        if (map[nx][ny] == WALL) { // 如果前面是墙壁，不能移动
            return false;
        }
        if (map[nx][ny] == BOX || map[nx][ny] == BOX_ON_GOAL) { // 如果前面是箱子，需要判断箱子前面是否有空地或目标点
            int nnx = nx + dx[dir];
            int nny = ny + dy[dir];
            if (map[nnx][nny] == EMPTY || map[nnx][nny] == GOAL) { // 如果箱子前面是空地或目标点，可以移动
                return true;
            }
            else { // 否则不能移动
                return false;
            }
        }
        return true; // 如果前面是空地或目标点，可以移动
    }

    // 根据方向生成下一个状态
    State* move(int dir) {
        char new_map[ROW][COL];
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                new_map[i][j] = map[i][j];
            }
        }
        int new_x = man_x + dx[dir];
        int new_y = man_y + dy[dir];
        if (new_map[new_x][new_y] == BOX || new_map[new_x][new_y] == BOX_ON_GOAL) { // 如果前面是箱子，需要把箱子也移动
            int new_xx = new_x + dx[dir];
            int new_yy = new_y + dy[dir];
            if (new_map[new_xx][new_yy] == EMPTY) { // 如果箱子前面是空地，把箱子放到空地上
                new_map[new_xx][new_yy] = BOX;
            }
            else { // 如果箱子前面是目标点，把箱子放到目标点上
                new_map[new_xx][new_yy] = BOX_ON_GOAL;
            }
        }
        if (new_map[man_x][man_y] == MAN) { // 如果原来的位置是人，把它改成空地
            new_map[man_x][man_y] = EMPTY;
        }
        else { // 如果原来的位置是人在目标点上，把它改成目标点
            new_map[man_x][man_y] = GOAL;
        }
        if (new_map[new_x][new_y] == EMPTY) { // 如果新的位置是空地，把它改成人
            new_map[new_x][new_y] = MAN;
        }
        else { // 如果新的位置是目标点，把它改成人在目标点上
            new_map[new_x][new_y] = MAN_ON_GOAL;
        }
        return new State(new_map, new_x, new_y, this, op[dir]); // 返回新的状态
    }

    // 重载==运算符，用于判断两个状态是否相同
    bool operator==(const State& s) const {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (map[i][j] != s.map[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    // 重载<运算符，用于哈希表的比较
    bool operator<(const State& s) const {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (map[i][j] < s.map[i][j]) {
                    return true;
                }
                else if (map[i][j] > s.map[i][j]) {
                    return false;
                }
            }
        }
        return false;
    }

};

// 定义哈希函数，用于哈希表的存储
struct HashFunc {
    size_t operator()(const State& s) const {
        size_t hash = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                hash = hash * 10 + s.map[i][j];
            }
        }
        return hash;
    }
};

// 定义初始地图数组
char init_map[ROW][COL] =
{
    {'#', '#', '#', '#', '#', '#', '#'},
    {'#', '.', '.', '.', '.', '.', '#'},
    {'#', '.', '$', '@', '.', '.', '#'},
    {'#', '.', '.', '.', '*', '.', '#'},
    {'#', '.', '.', '.', '.', '.', '#'},
    {'#', '#','#', '#', '#', '#', '#', '#', '#'}
};

// 定义初始人的位置
int init_x = 2;
int init_y = 3;

// 定义广度优先搜索函数，返回目标状态或空指针
State* bfs() {
    queue<State*> q; // 定义队列
    unordered_set<State, HashFunc> visited; // 定义哈希表
    State* start = new State(init_map, init_x, init_y, NULL, ' '); // 定义初始状态
    q.push(start); // 把初始状态入队
    visited.insert(*start); // 把初始状态加入哈希表
    while (!q.empty()) { // 当队列不为空时循环
        State* cur = q.front(); // 取出队首元素
        q.pop(); // 出队
        if (cur->isGoal()) { // 如果是目标状态，返回该状态
            return cur;
        }
        for (int i = 0; i < 4; i++) { // 对于四个方向
            if (cur->canMove(i)) { // 如果可以移动或推动箱子
                State* next = cur->move(i); // 生成下一个状态
                if (visited.find(*next) == visited.end()) { // 如果下一个状态没有被访问过
                    q.push(next); // 入队
                    visited.insert(*next); // 加入哈希表
                }
            }
        }
    }
    return NULL; // 如果没有找到目标状态，返回空指针
}

// 定义输出解法函数，打印操作序列或无解提示
void printSolution() {
    State* goal = bfs(); // 调用广度优先搜索函数，得到目标状态或空指针
    if (goal == NULL) { // 如果是空指针，表示无解
        cout << "No solution!" << endl;
    }
    else { // 如果是目标状态，回溯操作序列并输出
        stack<char> s; // 定义栈
        State* cur = goal; // 从目标状态开始回溯
        while (cur->prev != NULL) { // 当前驱状态不为空时循环
            s.push(cur->action); // 把当前操作压入栈中
            cur = cur->prev; // 回溯到前驱状态
        }
        while (!s.empty()) { // 当栈不为空时循环
            cout << s.top(); // 输出栈顶元素
            s.pop(); // 出栈
        }
        cout << endl;
    }
}

// 主函数，调用输出解法函数并释放内存
int main() {
    printSolution(); // 调用输出解法函数
    system("pause"); // 暂停程序，等待用户输入任意键继续
    return 0; // 返回0，表示程序正常结束
}