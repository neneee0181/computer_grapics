#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <iomanip>

#define ARRAY_LENGHT 4

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dis(0, 2);

bool checkList(int list[4][4]) {
	int status = 0;
	for (int i = 0; i < ARRAY_LENGHT; ++i) {
		for (int j = 0; j < ARRAY_LENGHT; ++j) {
			if (list[i][j] == 2) {
				status++;
			}
		}
	}

	if (status == 2) {
		return false;
	}

	return true;
}

void rand_s(int list[4][4]) {
	do {
		for (int i = 0; i < ARRAY_LENGHT; ++i) {
			for (int j = 0; j < ARRAY_LENGHT; ++j) {
				list[i][j] = dis(gen);
			}
		}
	} while (checkList(list));
}

void print_s(int list_1[4][4], int list_2[4][4]) {
	cout << endl;
	cout << "1번 행렬\t\t\t\t\t";
	cout << "2번 행렬" << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cout << list_1[i][j] << " ";
		}
		cout << "\t\t\t\t\t";
		for (int j = 0; j < 4; ++j) {
			cout << list_2[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl << endl;
}

void plus_s(int list_1[4][4], int list_2[4][4]) {
	cout << "더한 행렬 값" << endl;

	int sum = 0;

	for (int j = 0; j < 4; ++j) {
		for (int k = 0; k < 4; ++k) {
			sum = list_1[j][k] + list_2[j][k];
			cout << sum << " ";
		}
		cout << endl;
	}
}

void minus_s(int list_1[4][4], int list_2[4][4]) {
	cout << "뺀 행렬 값" << endl;

	int sum = 0;

	for (int j = 0; j < 4; ++j) {
		for (int k = 0; k < 4; ++k) {
			sum = list_1[j][k] - list_2[j][k];
			cout << right << setw(2) << sum << " ";
		}
		cout << endl;
	}
}

void multiply(int list_1[4][4], int list_2[4][4]) {
	cout << "곱한 행렬 값" << endl;

	int sum = 0;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			sum = 0;
			for (int k = 0; k < 4; ++k) {
				sum += list_1[i][k] * list_2[k][j];
			}
			cout << sum << " ";
		}
		cout << endl;
	}
}

int determinant(int matrix[4][4]) {
	int det = 0;
	for (int i = 0; i < 4; i++) {
		int submatrix[3][3];
		for (int j = 1; j < 4; j++) {
			int subcol = 0;
			for (int k = 0; k < 4; k++) {
				if (k == i) continue;
				submatrix[j - 1][subcol] = matrix[j][k];
				subcol++;
			}
		}
		// 행렬식 계산
		int subdet = submatrix[0][0] * (submatrix[1][1] * submatrix[2][2] - submatrix[1][2] * submatrix[2][1])
			- submatrix[0][1] * (submatrix[1][0] * submatrix[2][2] - submatrix[1][2] * submatrix[2][0])
			+ submatrix[0][2] * (submatrix[1][0] * submatrix[2][1] - submatrix[1][1] * submatrix[2][0]);
		det += (i % 2 == 0 ? 1 : -1) * matrix[0][i] * subdet;
	}
	return det;
}

void print_result(int list_1[4][4], int list_2[4][4]) {
	cout << "1번 행렬의 행렬식 값: " << determinant(list_1) << endl;
	cout << "2번 행렬의 행렬식 값: " << determinant(list_2) << endl;
}

void transpose_matrix(int list_1[4][4], int list_2[4][4]) {
	// 전치 행렬 생성
	int transposed_1[4][4];
	int transposed_2[4][4];

	// 첫 번째 행렬 전치
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			transposed_1[j][i] = list_1[i][j];
		}
	}

	// 두 번째 행렬 전치
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			transposed_2[j][i] = list_2[i][j];
		}
	}

	// 전치 행렬 출력
	cout << "1번 행렬의 전치 행렬:" << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cout << transposed_1[i][j] << " ";
		}
		cout << endl;
	}

	cout << "2번 행렬의 전치 행렬:" << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cout << transposed_2[i][j] << " ";
		}
		cout << endl;
	}
}

void print_even_odd(int list[4][4], int mode) {
	// mode 0: 짝수만 출력, mode 1: 홀수만 출력, mode 2: 전체 출력
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (mode == 0 && list[i][j] % 2 == 0) {
				cout << list[i][j] << " ";
			}
			else if (mode == 1 && list[i][j] % 2 != 0) {
				cout << list[i][j] << " ";
			}
			else if (mode == 2) {
				cout << list[i][j] << " ";
			}
			else {
				cout << "  ";
			}
		}
		cout << endl;
	}
}

void print_even_odd_matrices(int list_1[4][4], int list_2[4][4], int& mode) {
	cout << "1번 행렬" << endl;
	print_even_odd(list_1, mode);
	cout << "2번 행렬" << endl;
	print_even_odd(list_2, mode);
	// 상태 변경 (짝수 -> 홀수 -> 전체)
	mode = (mode + 1) % 3;
}

void multiply_by_number(int list_1[4][4], int list_2[4][4], int number) {
	// 각 행렬의 모든 원소에 number를 곱함
	cout << "1번 행렬의 각 원소에 " << number << " 곱하기:" << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cout << list_1[i][j] * number << " ";
		}
		cout << endl;
	}

	cout << "2번 행렬의 각 원소에 " << number << " 곱하기:" << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cout << list_2[i][j] * number << " ";
		}
		cout << endl;
	}
}

int main() {
	int list_1[4][4] = { 0, };
	int list_2[4][4] = { 0, };
	int mode = 0;  // 짝수, 홀수, 전체를 나타내는 상태 변수

	rand_s(list_1);
	rand_s(list_2);

	print_s(list_1, list_2);

	while (true) {
		char order;

		cout << "m, a, d, r, t, e, s, number 1~9, q : ";
		cin >> order;

		if (order >= '1' && order <= '9') {
			int number = order - '0';  // 문자를 숫자로 변환
			print_s(list_1, list_2);
			multiply_by_number(list_1, list_2, number);
		}
		else {
			switch (order)
			{
			case 'e':
				cout << "홀, 짝 출력" << endl;
				print_s(list_1, list_2);
				print_even_odd_matrices(list_1, list_2, mode);
				break;
			case 't':
				cout << "행렬식 -> 전치행렬 값" << endl;
				print_s(list_1, list_2);
				transpose_matrix(list_1, list_2);
				break;
			case 'r':
				cout << "행렬식의 값" << endl;
				print_s(list_1, list_2);
				print_result(list_1, list_2);
				break;
			case 'd':
				cout << "행렬 빼기" << endl;
				print_s(list_1, list_2);
				minus_s(list_1, list_2);
				break;
			case 'a':
				cout << "행렬 더하기" << endl;
				print_s(list_1, list_2);
				plus_s(list_1, list_2);
				break;
			case 'm':
				cout << "행렬 곱하기" << endl;
				print_s(list_1, list_2);
				multiply(list_1, list_2);
				break;
			case 's':
				rand_s(list_1);
				rand_s(list_2);

				print_s(list_1, list_2);
				break;
			case 'q':
				exit(0);
				break;
			default:
				break;
			}
		}
	}
}
