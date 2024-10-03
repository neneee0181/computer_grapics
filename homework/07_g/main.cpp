#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h> 
#endif

using namespace std;

char board[30][30];
int x_start = 0, y_start = 0, x_end = 0, y_end = 0;
int x_start_2 = 0, y_start_2 = 0, x_end_2 = 0, y_end_2 = 0;

void setColorRed() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
#else
    cout << "\033[31m";
#endif
}

void resetColor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
#else
    cout << "\033[0m";
#endif
}

void print_board() {
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            if (board[i][j] == '#') {
                setColorRed();
            }
            cout << board[i][j] << " ";
            resetColor();
        }
        cout << endl;
    }
}

int wrap_around(int coordinate, int max_value) {
    return (coordinate + max_value) % max_value;
}

void draw_rectangle1(int x_start, int y_start, int x_end, int y_end) {
    if (x_start > x_end) swap(x_start, x_end);
    if (y_start > y_end) swap(y_start, y_end);

    for (int i = y_start; i <= y_end; ++i) {
        for (int j = x_start; j <= x_end; ++j) {
            int wrapped_i = wrap_around(i, 30);
            int wrapped_j = wrap_around(j, 30);

            if (board[wrapped_i][wrapped_j] == 'X') {
                board[wrapped_i][wrapped_j] = '#';
            }
            else {
                board[wrapped_i][wrapped_j] = '0';
            }
        }
    }
}

void draw_rectangle2(int x_start, int y_start, int x_end, int y_end) {
    if (x_start > x_end) swap(x_start, x_end);
    if (y_start > y_end) swap(y_start, y_end);

    for (int i = y_start; i <= y_end; ++i) {
        for (int j = x_start; j <= x_end; ++j) {
            int wrapped_i = wrap_around(i, 30);
            int wrapped_j = wrap_around(j, 30);

            if (board[wrapped_i][wrapped_j] == '0') {
                board[wrapped_i][wrapped_j] = '#';
            }
            else {
                board[wrapped_i][wrapped_j] = 'X';
            }
        }
    }
}

void reset_board() {
    fill(&board[0][0], &board[0][0] + 30 * 30, '.');
}

void _move(char input) {
    switch (input) {
    case 'x':
        x_start = x_start - 1;
        x_end = x_end - 1;
        break;
    case 'X':
        x_start = x_start + 1;
        x_end = x_end + 1;
        break;
    case 'y':
        y_start = y_start - 1;
        y_end = y_end - 1;
        break;
    case 'Y':
        y_start = y_start + 1;
        y_end = y_end + 1;
        break;
    case 's':
        y_start--;
        x_start--;
        y_end++;
        x_end++;
        break;
    case 'S':
        y_start++;
        x_start++;
        y_end--;
        x_end--;
        break;
    case 'w':
        x_start_2 = x_start_2 - 1;
        x_end_2 = x_end_2 - 1;
        break;
    case 'W':
        x_start_2 = x_start_2 + 1;
        x_end_2 = x_end_2 + 1;
        break;
    case 'a':
        y_start_2 = y_start_2 - 1;
        y_end_2 = y_end_2 - 1;
        break;
    case 'A':
        y_start_2 = y_start_2 + 1;
        y_end_2 = y_end_2 + 1;
        break;
    case 'd':
        x_start_2++;
        y_start_2++;
        x_end_2--;
        y_end_2--;
        break;
    case 'D':
        x_start_2--;
        y_start_2--;
        x_end_2++;
        y_end_2++;
        break;
    case 'r':
        cout << "1번째 도형 좌표를 입력하세요 x1 y1 x2 y2 : ";
        cin >> x_start >> y_start >> x_end >> y_end;

        cout << "2번째 도형 좌표를 입력하세요 x1 y1 x2 y2 : ";
        cin >> x_start_2 >> y_start_2 >> x_end_2 >> y_end_2;
        break;
    default:
        break;
    }
}

int main() {
    reset_board();

    cout << "1번째 도형 좌표를 입력하세요 x1 y1 x2 y2 : ";
    cin >> x_start >> y_start >> x_end >> y_end;

    cout << "2번째 도형 좌표를 입력하세요 x1 y1 x2 y2 : ";
    cin >> x_start_2 >> y_start_2 >> x_end_2 >> y_end_2;

    while (true) {
        reset_board();
        draw_rectangle1(x_start, y_start, x_end, y_end);
        draw_rectangle2(x_start_2, y_start_2, x_end_2, y_end_2);

        print_board();
        cout << endl;

        char input;
        cout << "submit : ";
        cin >> input;

        _move(input);
    }

    return 0;
}