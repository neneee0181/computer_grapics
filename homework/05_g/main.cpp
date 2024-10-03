#include <iostream>
#include <vector>

using namespace std;

char board[30][30];
int x_start = 0, y_start = 0, x_end = 0, y_end = 0;

void print_board() {
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

void draw_rectangle(int x_start, int y_start, int x_end, int y_end) {
    if (x_start > x_end) swap(x_start, x_end);
    if (y_start > y_end) swap(y_start, y_end);

    for (int i = y_start; i <= y_end; ++i) {
        for (int j = x_start; j <= x_end; ++j) {
            int wrapped_i = (i + 30) % 30;
            int wrapped_j = (j + 30) % 30;

            if (i >= 0 && i < 30 && j >= 0 && j < 30) {
                board[i][j] = '0';
            }

            if (wrapped_i != i || wrapped_j != j) {
                board[wrapped_i][wrapped_j] = '0';
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
    case 'i':
        x_start--;
        x_end++;
        break;
    case 'I':
        x_start++;
        x_end--;
        break;
    case 'j':
        y_start--;
        y_end++;
        break;
    case 'J':
        y_start++;
        y_end--;
        break;
    case 'a':
        x_start--;
        x_end++;
        y_start++;
        y_end--;
        break;
    case 'A':
        x_start++;
        x_end--;
        y_start--;
        y_end++;
        break;
    case 'm': {
        int x_result = x_end - x_start;
        int y_result = y_end - y_start;

        cout << "면적 : " << x_result << " x " << y_result << " = " << x_result * y_result << endl;
    }
            break;
    case 'n': {
        int x_result = x_end - x_start + 1; // 사각형의 가로 길이
        int y_result = y_end - y_start + 1; // 사각형의 세로 길이
        int rectangle_area = x_result * y_result;
        int total_area = 30 * 30;

        double area_ratio = (static_cast<double>(rectangle_area) / total_area) * 100;

        cout << "사각형 면적 : " << x_result << " x " << y_result << " = " << rectangle_area << endl;
        cout << "전체 면적 : 30 x 30 = 900" << endl;
        cout << "면적 비율 : (" << rectangle_area << " / 900) * 100 = " << area_ratio << "%" << endl;
    }
            break;
    case 'r':
        cout << "좌표를 입력하세요 x1 y1 x2 y2 : ";
        cin >> x_start >> y_start >> x_end >> y_end;
        break;
    default:
        break;
    }
}

int main() {
    reset_board();

    cout << "좌표를 입력하세요 x1 y1 x2 y2 : ";
    cin >> x_start >> y_start >> x_end >> y_end;

    while (true) {

        //system("cls");

        reset_board();
        draw_rectangle(x_start, y_start, x_end, y_end);

        print_board();
        cout << endl;

        char input;
        cout << "submit : ";
        cin >> input;

        _move(input);
    }

    return 0;
}
