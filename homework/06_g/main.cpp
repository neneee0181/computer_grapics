#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <conio.h>  // getch()를 사용하기 위해 추가

using namespace std;

#define mapSize 30
#define obstacleCount 30 // 장애물 개수

char map[mapSize][mapSize];
bool visited[mapSize][mapSize];
int playerX = 0, playerY = 0;

int dx[4] = { -1, 1, 0, 0 }; // 상, 하, 좌, 우
int dy[4] = { 0, 0, -1, 1 };

void map_start() {
    fill(&map[0][0], &map[0][0] + mapSize * mapSize, '1');
    fill(&visited[0][0], &visited[0][0] + mapSize * mapSize, false);
}

void map_print() {
    system("cls"); // 화면을 지워서 보드를 갱신해 보여줌
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            if (i == playerX && j == playerY) {
                cout << "\033[31m*\033[0m "; // *를 빨간색으로 출력
            }
            else if (map[i][j] == 'X') {
                cout << "\033[34mX\033[0m "; // X를 파란색으로 출력
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

    vector<int> directions = { 1, 3, 2 }; // 하(1), 우(3), 상(2)만 허용, 좌(0)를 제거

    random_shuffle(directions.begin(), directions.end());

    for (int i = 0; i < 3; ++i) {  // 방향은 3가지로만 제한
        int dir = directions[i];
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // 조건 1: 경로는 한쪽 방향으로 최장 5칸 이상 계속 이동할 수 없다.
        if (dir == lastDirection && stepsInDirection >= 5) {
            continue;
        }

        // 조건 3: 경로는 장애물을 만나면 다른 방향으로 전환한다.
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize && map[nx][ny] == 'X') {
            continue;
        }

        // 방문 가능 여부 확인
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize && !visited[nx][ny]) {
            bool newDirectionChanged = directionChanged || (dir != lastDirection && lastDirection != -1);

            // 조건 2: 방향 전환이 한 번 이상 이루어졌는지 확인
            if (newDirectionChanged || (stepsInDirection < 5)) {
                if (create_path(nx, ny, dir, dir == lastDirection ? stepsInDirection + 1 : 1, newDirectionChanged)) {
                    return true;
                }
            }
        }
    }

    map[x][y] = '1'; // 실패하면 길을 되돌림
    return false;
}

void place_obstacles() {
    int count = 0;

    // 비경로 구역 리스트 생성
    vector<pair<int, int>> nonPathZones;
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            if (!visited[i][j] && map[i][j] == '1') {
                nonPathZones.push_back({ i, j });
            }
        }
    }

    // 장애물을 비경로 구역에 랜덤하게 배치
    while (count < obstacleCount && !nonPathZones.empty()) {
        int randIndex = rand() % nonPathZones.size();
        int randX = nonPathZones[randIndex].first;
        int randY = nonPathZones[randIndex].second;

        map[randX][randY] = 'X';
        count++;
        nonPathZones.erase(nonPathZones.begin() + randIndex); // 사용한 위치 제거
    }
}

void move_player(char direction) {
    int newX = playerX, newY = playerY;

    switch (direction) {
    case 'w': newX = playerX - 1; break; // 위로 이동
    case 's': newX = playerX + 1; break; // 아래로 이동
    case 'a': newY = playerY - 1; break; // 왼쪽 이동 (사용자 조작은 허용)
    case 'd': newY = playerY + 1; break; // 오른쪽 이동
    default: return;
    }

    if (newX >= 0 && newX < mapSize && newY >= 0 && newY < mapSize && map[newX][newY] == '0') {
        playerX = newX;
        playerY = newY;
    }
}

void reset_game() {
    playerX = 0; // 플레이어 위치 초기화
    playerY = 0;
    map_start(); // 맵 초기화
    create_path(0, 0); // 새로운 경로 생성
    place_obstacles(); // 새로운 장애물 배치
}

int main() {
    srand(time(0));
    map_start();
    create_path(0, 0); // 위, 아래, 오른쪽 경로 생성
    place_obstacles();

    while (true) {
        map_print();
        char input = _getch();

        if (input == 'q') { // 게임 종료
            exit(0);
            break;
        }
        else if (input == 'r') { // r을 누르면 리셋
            reset_game();
        }
        else {
            move_player(input); // 플레이어 이동
        }
    }

    return 0;
}
