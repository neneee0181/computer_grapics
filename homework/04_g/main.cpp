#include <iostream>
#include <vector>
#include <algorithm> 
#include <ctime>    
#include <windows.h> 
#include <unordered_map>

using namespace std;

struct char_m {
    char ch;
    WORD color;
};

char_m board[5][5];
char_m result_board[5][5];

void setColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void printCharWithColor(const char_m& cm) {
    setColor(cm.color);
    cout << cm.ch;
    resetColor();
}

// 문자에 따른 색상 매핑 생성
WORD getColorForChar(char c) {
    static unordered_map<char, WORD> colorMap = {
        {'A', FOREGROUND_RED | FOREGROUND_INTENSITY},        // A - 빨간색
        {'B', FOREGROUND_GREEN | FOREGROUND_INTENSITY},      // B - 초록색
        {'C', FOREGROUND_BLUE | FOREGROUND_INTENSITY},       // C - 파란색
        {'D', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},  // D - 노란색
        {'E', FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY}, // E - 청록색
        {'F', FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},   // F - 자홍색
        {'G', FOREGROUND_GREEN},      // G - 초록색 (어두운)
        {'H', FOREGROUND_BLUE},       // H - 파란색 (어두운)
        {'I', FOREGROUND_RED},        // I - 빨간색 (어두운)
        {'J', FOREGROUND_RED | FOREGROUND_GREEN}, // J - 갈색
        {'K', FOREGROUND_GREEN | FOREGROUND_BLUE}, // K - 청록색 (어두운)
        {'L', FOREGROUND_RED | FOREGROUND_BLUE}    // L - 보라색 (어두운)
    };

    // 매핑에 없는 문자에 대한 기본 색상
    return colorMap.count(c) ? colorMap[c] : 7; // 7은 기본 흰색
}

void print_board() {
    char alpha = 'a';

    cout << "  ";
    for (int i = 0; i < 5; ++i) {
        cout << alpha++ << " ";
    }
    cout << endl;

    int num = 1;

    for (int i = 0; i < 5; ++i) {
        cout << num++ << " ";
        for (int j = 0; j < 5; ++j) {
            printCharWithColor(board[i][j]);
            cout << " ";
        }
        cout << endl;
    }

    cout << endl;
}

void reset() {

    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            board[i][j] = { '*', 7 };
            result_board[i][j] = { '*', 7 };
        }
    }

    vector<char> chars = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L' };

    vector<pair<int, int>> positions;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            positions.push_back(make_pair(i, j));
        }
    }

    random_shuffle(positions.begin(), positions.end());

    int index = 0;
    for (char c : chars) {
        for (int k = 0; k < 2; k++) {  
            int row = positions[index].first;
            int col = positions[index].second;
            result_board[row][col] = { c, getColorForChar(c) };
            index++;
        }
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (result_board[i][j].ch < 'A' || result_board[i][j].ch > 'Z') {
                result_board[i][j] = { '@', FOREGROUND_RED | FOREGROUND_INTENSITY };
            }
        }
    }
}

int main() {
    
    reset();

    int count_g = 40;
    int score = 0;

    while (true) {

        cout << "가능 횟수 : " << count_g << endl;
        cout << "점수 : " << score << endl << endl;

        print_board();

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                printCharWithColor(result_board[i][j]);
                cout << " ";
            }
            cout << endl;
        }
        

        string input1, input2;
        cout << "좌표를 입력하세요 : ";
        cin >> input1 >> input2;

        if (input1[0] == 'r' && input2[0] == '-') {
            reset();
        }else if (input1.length() == 2 && input2.length() == 2) {
            char col1 = toupper(input1[0]); 
            char row1 = input1[1];         

            int colIndex1 = col1 - 'A';
            int rowIndex1 = row1 - '1';

            char col2 = toupper(input2[0]);
            char row2 = input2[1];

            int colIndex2 = col2 - 'A';
            int rowIndex2 = row2 - '1';

            if (colIndex1 >= 0 && colIndex1 < 5 && rowIndex1 >= 0 && rowIndex1 < 5 &&
                colIndex2 >= 0 && colIndex2 < 5 && rowIndex2 >= 0 && rowIndex2 < 5) {

                board[rowIndex1][colIndex1] = result_board[rowIndex1][colIndex1];
                board[rowIndex2][colIndex2] = result_board[rowIndex2][colIndex2];

                print_board();

                if (board[rowIndex1][colIndex1].ch == '@') {
                    for (int i = 0; i < 5; ++i) {
                        for (int j = 0; j < 5; ++j) {
                            if (result_board[i][j].ch == board[rowIndex2][colIndex2].ch) {
                                board[i][j] = result_board[i][j];
                                rowIndex1 = i;
                                colIndex1 = j;
                            }
                        }
                    }
                }
                else if (board[rowIndex2][colIndex2].ch == '@') {
                    for (int i = 0; i < 5; ++i) {
                        for (int j = 0; j < 5; ++j) {
                            if (result_board[i][j].ch == board[rowIndex1][colIndex1].ch) {
                                board[i][j] = result_board[i][j];
                                rowIndex2 = i;
                                colIndex2 = j;
                            }
                        }
                    }
                }

                if (board[rowIndex1][colIndex1].ch != board[rowIndex2][colIndex2].ch) {
                    board[rowIndex1][colIndex1].ch = '*';
                    board[rowIndex1][colIndex1].color = getColorForChar(7);
                    board[rowIndex2][colIndex2].ch = '*';
                    board[rowIndex2][colIndex2].color = getColorForChar(7);
                }
                else
                {
                    score += 100;
                }

                if (count_g == 0) {
                    cout << "게임 종료" << endl;
                }

                count_g--;
            }
            else {
                cout << "잘못된 좌표입니다. 다시 시도하세요." << endl;
            }
        }
        else {
            cout << "잘못된 입력값" << endl;
        }
    }

    return 0;
}
