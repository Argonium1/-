#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
using namespace std;

// �����ͼ��С
const int ROW = 10;
const int COL = 12;

// �������
const char WALL = '#';
const char EMPTY = '.';
const char MAN = '@';
const char BOX = '$';
const char GOAL = '*';
const char MAN_ON_GOAL = '+';
const char BOX_ON_GOAL = '!';

// ���巽��
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

// ���巽������
const int dx[4] = { -1, 1, 0, 0 };
const int dy[4] = { 0, 0, -1, 1 };

// �����������
const char op[4] = { 'U', 'D', 'L', 'R' };

// ����״̬�ṹ��
struct State {
    char map[ROW][COL]; // ��ͼ����
    int man_x; // �˵ĺ�����
    int man_y; // �˵�������
    State* prev; // ǰ��״ָ̬��
    char action; // ǰһ������

    // ���캯��
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

    // �ж��Ƿ���Ŀ��״̬�����������Ӷ���Ŀ�����
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

    // �ж��Ƿ������ĳ�������ƶ����ƶ�����
    bool canMove(int dir) {
        int nx = man_x + dx[dir];
        int ny = man_y + dy[dir];
        if (map[nx][ny] == WALL) { // ���ǰ����ǽ�ڣ������ƶ�
            return false;
        }
        if (map[nx][ny] == BOX || map[nx][ny] == BOX_ON_GOAL) { // ���ǰ�������ӣ���Ҫ�ж�����ǰ���Ƿ��пյػ�Ŀ���
            int nnx = nx + dx[dir];
            int nny = ny + dy[dir];
            if (map[nnx][nny] == EMPTY || map[nnx][nny] == GOAL) { // �������ǰ���ǿյػ�Ŀ��㣬�����ƶ�
                return true;
            }
            else { // �������ƶ�
                return false;
            }
        }
        return true; // ���ǰ���ǿյػ�Ŀ��㣬�����ƶ�
    }

    // ���ݷ���������һ��״̬
    State* move(int dir) {
        char new_map[ROW][COL];
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                new_map[i][j] = map[i][j];
            }
        }
        int new_x = man_x + dx[dir];
        int new_y = man_y + dy[dir];
        if (new_map[new_x][new_y] == BOX || new_map[new_x][new_y] == BOX_ON_GOAL) { // ���ǰ�������ӣ���Ҫ������Ҳ�ƶ�
            int new_xx = new_x + dx[dir];
            int new_yy = new_y + dy[dir];
            if (new_map[new_xx][new_yy] == EMPTY) { // �������ǰ���ǿյأ������ӷŵ��յ���
                new_map[new_xx][new_yy] = BOX;
            }
            else { // �������ǰ����Ŀ��㣬�����ӷŵ�Ŀ�����
                new_map[new_xx][new_yy] = BOX_ON_GOAL;
            }
        }
        if (new_map[man_x][man_y] == MAN) { // ���ԭ����λ�����ˣ������ĳɿյ�
            new_map[man_x][man_y] = EMPTY;
        }
        else { // ���ԭ����λ��������Ŀ����ϣ������ĳ�Ŀ���
            new_map[man_x][man_y] = GOAL;
        }
        if (new_map[new_x][new_y] == EMPTY) { // ����µ�λ���ǿյأ������ĳ���
            new_map[new_x][new_y] = MAN;
        }
        else { // ����µ�λ����Ŀ��㣬�����ĳ�����Ŀ�����
            new_map[new_x][new_y] = MAN_ON_GOAL;
        }
        return new State(new_map, new_x, new_y, this, op[dir]); // �����µ�״̬
    }

    // ����==������������ж�����״̬�Ƿ���ͬ
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

    // ����<����������ڹ�ϣ��ıȽ�
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

// �����ϣ���������ڹ�ϣ��Ĵ洢
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

// �����ʼ��ͼ����
char init_map[ROW][COL] =
{
    {'#', '#', '#', '#', '#', '#', '#'},
    {'#', '.', '.', '.', '.', '.', '#'},
    {'#', '.', '$', '@', '.', '.', '#'},
    {'#', '.', '.', '.', '*', '.', '#'},
    {'#', '.', '.', '.', '.', '.', '#'},
    {'#', '#','#', '#', '#', '#', '#', '#', '#'}
};

// �����ʼ�˵�λ��
int init_x = 2;
int init_y = 3;

// ������������������������Ŀ��״̬���ָ��
State* bfs() {
    queue<State*> q; // �������
    unordered_set<State, HashFunc> visited; // �����ϣ��
    State* start = new State(init_map, init_x, init_y, NULL, ' '); // �����ʼ״̬
    q.push(start); // �ѳ�ʼ״̬���
    visited.insert(*start); // �ѳ�ʼ״̬�����ϣ��
    while (!q.empty()) { // �����в�Ϊ��ʱѭ��
        State* cur = q.front(); // ȡ������Ԫ��
        q.pop(); // ����
        if (cur->isGoal()) { // �����Ŀ��״̬�����ظ�״̬
            return cur;
        }
        for (int i = 0; i < 4; i++) { // �����ĸ�����
            if (cur->canMove(i)) { // ��������ƶ����ƶ�����
                State* next = cur->move(i); // ������һ��״̬
                if (visited.find(*next) == visited.end()) { // �����һ��״̬û�б����ʹ�
                    q.push(next); // ���
                    visited.insert(*next); // �����ϣ��
                }
            }
        }
    }
    return NULL; // ���û���ҵ�Ŀ��״̬�����ؿ�ָ��
}

// ��������ⷨ��������ӡ�������л��޽���ʾ
void printSolution() {
    State* goal = bfs(); // ���ù�����������������õ�Ŀ��״̬���ָ��
    if (goal == NULL) { // ����ǿ�ָ�룬��ʾ�޽�
        cout << "No solution!" << endl;
    }
    else { // �����Ŀ��״̬�����ݲ������в����
        stack<char> s; // ����ջ
        State* cur = goal; // ��Ŀ��״̬��ʼ����
        while (cur->prev != NULL) { // ��ǰ��״̬��Ϊ��ʱѭ��
            s.push(cur->action); // �ѵ�ǰ����ѹ��ջ��
            cur = cur->prev; // ���ݵ�ǰ��״̬
        }
        while (!s.empty()) { // ��ջ��Ϊ��ʱѭ��
            cout << s.top(); // ���ջ��Ԫ��
            s.pop(); // ��ջ
        }
        cout << endl;
    }
}

// ����������������ⷨ�������ͷ��ڴ�
int main() {
    printSolution(); // ��������ⷨ����
    system("pause"); // ��ͣ���򣬵ȴ��û��������������
    return 0; // ����0����ʾ������������
}