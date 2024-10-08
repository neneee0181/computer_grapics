#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// 전역 변수들
vector<string> originalLines;  // 파일에서 읽어온 원본 데이터 줄 저장
vector<string> currentLines;   // 현재 출력 상태를 저장
vector<string> modifiedLines;  // 특정 명령어에 의해 수정된 라인을 임시 저장
bool isModifiedC = false;      // C 명령어의 상태를 나타내는 플래그
bool isModifiedE = false;      // E 명령어의 상태를 나타내는 플래그
bool isModifiedG = false;      // G 명령어의 상태를 나타내는 플래그
bool ascending = true;         // 오름차순 / 내림차순 플래그
bool originalOrder = true;     // 원본 순서로 되돌리는 플래그

// 파일을 읽어오는 함수
void readFile(const string& fileName) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cerr << "파일을 열 수 없습니다." << endl;
        exit(1);
    }

    string line;
    originalLines.clear();
    while (getline(inputFile, line)) {
        originalLines.push_back(line);
    }
    inputFile.close();
    currentLines = originalLines;
}

// 현재 라인들을 출력하는 함수
void printCurrentLines() {
    for (const auto& line : currentLines) {
        cout << line << endl;
    }
}

// 대문자로 시작되는 단어를 색상 변경하여 출력하고, 다시 원상태로 돌리는 기능
void commandC() {
    if (isModifiedC) {
        currentLines = modifiedLines;
        isModifiedC = false;
        printCurrentLines();
        return;
    }

    modifiedLines = currentLines; // 원래 상태 저장
    int count = 0;
    for (auto& line : currentLines) {
        istringstream iss(line);
        string word;
        string newLine;
        while (iss >> word) {
            if (isupper(word[0])) {
                newLine += "\033[31m" + word + "\033[0m ";  // 대문자로 시작하는 단어를 빨간색으로 출력
                count++;
            }
            else {
                newLine += word + " ";
            }
        }
        line = newLine;
    }
    isModifiedC = true;
    printCurrentLines();
    cout << "대문자로 시작하는 단어 개수: " << count << endl;
}

// 한 줄 전체를 거꾸로 출력하기
void commandD() {
    for (auto& line : currentLines) {
        reverse(line.begin(), line.end());
    }
    printCurrentLines();
}

// 3문자 후 2개의 @ 삽입하고 토글 기능 추가
void commandE() {
    if (isModifiedE) {
        currentLines = modifiedLines;
        isModifiedE = false;
        printCurrentLines();
        return;
    }

    modifiedLines = currentLines; // 원래 상태 저장
    for (auto& line : currentLines) {
        string newLine;
        for (size_t i = 0; i < line.size(); i++) {
            newLine += line[i];
            if ((i + 1) % 3 == 0) {
                newLine += "@@";
            }
        }
        line = newLine;
    }
    isModifiedE = true;
    printCurrentLines();
}

// 공백 기준 모든 단어 거꾸로 출력
void commandF() {
    for (auto& line : currentLines) {
        istringstream iss(line);
        string word, newLine;
        while (iss >> word) {
            reverse(word.begin(), word.end());
            newLine += word + " ";
        }
        line = newLine;
    }
    printCurrentLines();
}

// 특정 문자를 다른 문자로 바꾸고 원상태로 되돌리기
void commandG() {
    static char lastOldChar, lastNewChar; // 마지막으로 바꾼 문자들 저장

    if (isModifiedG) {
        for (auto& line : currentLines) {
            replace(line.begin(), line.end(), lastNewChar, lastOldChar); // 이전 상태로 복구
        }
        isModifiedG = false;
        printCurrentLines();
        return;
    }

    char oldChar, newChar;
    cout << "바꿀 문자와 새로운 문자를 입력하세요: ";
    cin >> oldChar >> newChar;
    modifiedLines = currentLines; // 원래 상태 저장

    for (auto& line : currentLines) {
        replace(line.begin(), line.end(), oldChar, newChar);
    }

    lastOldChar = oldChar;
    lastNewChar = newChar;
    isModifiedG = true;
    printCurrentLines();
}

// 각 줄의 단어 개수 출력
void commandH() {
    for (const auto& line : currentLines) {
        istringstream iss(line);
        int wordCount = 0;
        string word;
        while (iss >> word) {
            wordCount++;
        }
        cout << "단어 개수: " << wordCount << " - " << line << endl;
    }
}

// 단어 개수에 따라 정렬, 오름차순 → 내림차순 → 원본 순서
void commandR() {
    if (originalOrder) {
        sort(currentLines.begin(), currentLines.end(), [](const string& a, const string& b) {
            istringstream issA(a), issB(b);
            int countA = distance(istream_iterator<string>(issA), istream_iterator<string>());
            int countB = distance(istream_iterator<string>(issB), istream_iterator<string>());
            return countA < countB;
            });
        originalOrder = false;
        ascending = false;
    }
    else if (!ascending) {
        sort(currentLines.begin(), currentLines.end(), [](const string& a, const string& b) {
            istringstream issA(a), issB(b);
            int countA = distance(istream_iterator<string>(issA), istream_iterator<string>());
            int countB = distance(istream_iterator<string>(issB), istream_iterator<string>());
            return countA > countB;
            });
        ascending = true;
    }
    else {
        currentLines = originalLines;
        originalOrder = true;
    }
    printCurrentLines();
}

// 특정 단어 찾아서 다른 색으로 출력하고 개수 계산
void commandS() {
    string searchWord;
    cout << "찾을 단어를 입력하세요: ";
    cin >> searchWord;

    int totalCount = 0;
    for (const auto& line : currentLines) {
        istringstream iss(line);
        string word;
        while (iss >> word) {
            string lowerWord = word;
            string lowerSearchWord = searchWord;

            transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
            transform(lowerSearchWord.begin(), lowerSearchWord.end(), lowerSearchWord.begin(), ::tolower);

            if (lowerWord.find(lowerSearchWord) != string::npos) {
                cout << "\033[32m" << word << "\033[0m ";  // 단어를 초록색으로 출력
                totalCount++;
            }
            else {
                cout << word << " ";
            }
        }
        cout << endl;
    }
    cout << "단어 \"" << searchWord << "\"의 총 개수: " << totalCount << endl;
}

int main() {
    string fileName;
    cout << "파일 이름을 입력하세요: ";
    cin >> fileName;

    readFile(fileName);
    printCurrentLines();

    char command;
    while (true) {
        cout << "\n명령어를 입력하세요 (c~q): ";
        cin >> command;
        switch (command) {
        case 'c':
            commandC();
            break;
        case 'd':
            commandD();
            break;
        case 'e':
            commandE();
            break;
        case 'f':
            commandF();
            break;
        case 'g':
            commandG();
            break;
        case 'h':
            commandH();
            break;
        case 'r':
            commandR();
            break;
        case 's':
            commandS();
            break;
        case 'q':
            cout << "프로그램을 종료합니다." << endl;
            return 0;
        default:
            cout << "올바르지 않은 명령어입니다. 다시 입력하세요." << endl;
            break;
        }
    }
}
