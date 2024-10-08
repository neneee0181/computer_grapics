#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <conio.h>  // getch()�� ����ϱ� ���� �߰�

using namespace std;

#define mapSize 30
#define obstacleCount 30 // ��ֹ� ����

char map[mapSize][mapSize];
bool visited[mapSize][mapSize];
int playerX = 0, playerY = 0;

int dx[4] = { -1, 1, 0, 0 }; // ��, ��, ��, ��
int dy[4] = { 0, 0, -1, 1 };

void map_start() {
    fill(&map[0][0], &map[0][0] + mapSize * mapSize, '1');
    fill(&visited[0][0], &visited[0][0] + mapSize * mapSize, false);
}

void map_print() {
    system("cls"); // ȭ���� ������ ���带 ������ ������
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            if (i == playerX && j == playerY) {
                cout << "\033[31m*\033[0m "; // *�� ���������� ���
            }
            else if (map[i][j] == 'X') {
                cout << "\033[34mX\033[0m "; // X�� �Ķ������� ���
            }
            else {
                cout << map[i][j] << " ";
            }
        }
        cout << endl;
    }
}

bool create_path(int x, int y, int lastDirection = -1, int stepsInDirection = 0, bool directionChanged = false) {
    map[x][y] = '0';
    visited[x][y] = true;

    if (x == mapSize - 1 && y == mapSize - 1) {
        return true;
    }

    vector<int> directions = { 1, 3, 2 }; // ��(1), ��(3), ��(2)�� ���, ��(0)�� ����

    random_shuffle(directions.begin(), directions.end());

    for (int i = 0; i < 3; ++i) {  // ������ 3�����θ� ����
        int dir = directions[i];
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // ���� 1: ��δ� ���� �������� ���� 5ĭ �̻� ��� �̵��� �� ����.
        if (dir == lastDirection && stepsInDirection >= 5) {
            continue;
        }

        // ���� 3: ��δ� ��ֹ��� ������ �ٸ� �������� ��ȯ�Ѵ�.
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize && map[nx][ny] == 'X') {
            continue;
        }

        // �湮 ���� ���� Ȯ��
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize && !visited[nx][ny]) {
            bool newDirectionChanged = directionChanged || (dir != lastDirection && lastDirection != -1);

            // ���� 2: ���� ��ȯ�� �� �� �̻� �̷�������� Ȯ��
            if (newDirectionChanged || (stepsInDirection < 5)) {
                if (create_path(nx, ny, dir, dir == lastDirection ? stepsInDirection + 1 : 1, newDirectionChanged)) {
                    return true;
                }
            }
        }
    }

    map[x][y] = '1'; // �����ϸ� ���� �ǵ���
    return false;
}

void place_obstacles() {
    int count = 0;

    // ���� ���� ����Ʈ ����
    vector<pair<int, int>> nonPathZones;
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            if (!visited[i][j] && map[i][j] == '1') {
                nonPathZones.push_back({ i, j });
            }
        }
    }

    // ��ֹ��� ���� ������ �����ϰ� ��ġ
    while (count < obstacleCount && !nonPathZones.empty()) {
        int randIndex = rand() % nonPathZones.size();
        int randX = nonPathZones[randIndex].first;
        int randY = nonPathZones[randIndex].second;

        map[randX][randY] = 'X';
        count++;
        nonPathZones.erase(nonPathZones.begin() + randIndex); // ����� ��ġ ����
    }
}

void move_player(char direction) {
    int newX = playerX, newY = playerY;

    switch (direction) {
    case 'w': newX = playerX - 1; break; // ���� �̵�
    case 's': newX = playerX + 1; break; // �Ʒ��� �̵�
    case 'a': newY = playerY - 1; break; // ���� �̵� (����� ������ ���)
    case 'd': newY = playerY + 1; break; // ������ �̵�
    default: return;
    }

    if (newX >= 0 && newX < mapSize && newY >= 0 && newY < mapSize && map[newX][newY] == '0') {
        playerX = newX;
        playerY = newY;
    }
}

void reset_game() {
    playerX = 0; // �÷��̾� ��ġ �ʱ�ȭ
    playerY = 0;
    map_start(); // �� �ʱ�ȭ
    create_path(0, 0); // ���ο� ��� ����
    place_obstacles(); // ���ο� ��ֹ� ��ġ
}

int main() {
    srand(time(0));
    map_start();
    create_path(0, 0); // ��, �Ʒ�, ������ ��� ����
    place_obstacles();

    while (true) {
        map_print();
        char input = _getch();

        if (input == 'q') { // ���� ����
            exit(0);
            break;
        }
        else if (input == 'r') { // r�� ������ ����
            reset_game();
        }
        else {
            move_player(input); // �÷��̾� �̵�
        }
    }

    return 0;
}
