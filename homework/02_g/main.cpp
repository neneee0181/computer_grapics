#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// ���� ������
vector<string> originalLines;  // ���Ͽ��� �о�� ���� ������ �� ����
vector<string> currentLines;   // ���� ��� ���¸� ����
vector<string> modifiedLines;  // Ư�� ��ɾ ���� ������ ������ �ӽ� ����
bool isModifiedC = false;      // C ��ɾ��� ���¸� ��Ÿ���� �÷���
bool isModifiedE = false;      // E ��ɾ��� ���¸� ��Ÿ���� �÷���
bool isModifiedG = false;      // G ��ɾ��� ���¸� ��Ÿ���� �÷���
bool ascending = true;         // �������� / �������� �÷���
bool originalOrder = true;     // ���� ������ �ǵ����� �÷���

// ������ �о���� �Լ�
void readFile(const string& fileName) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cerr << "������ �� �� �����ϴ�." << endl;
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

// ���� ���ε��� ����ϴ� �Լ�
void printCurrentLines() {
    for (const auto& line : currentLines) {
        cout << line << endl;
    }
}

// �빮�ڷ� ���۵Ǵ� �ܾ ���� �����Ͽ� ����ϰ�, �ٽ� �����·� ������ ���
void commandC() {
    if (isModifiedC) {
        currentLines = modifiedLines;
        isModifiedC = false;
        printCurrentLines();
        return;
    }

    modifiedLines = currentLines; // ���� ���� ����
    int count = 0;
    for (auto& line : currentLines) {
        istringstream iss(line);
        string word;
        string newLine;
        while (iss >> word) {
            if (isupper(word[0])) {
                newLine += "\033[31m" + word + "\033[0m ";  // �빮�ڷ� �����ϴ� �ܾ ���������� ���
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
    cout << "�빮�ڷ� �����ϴ� �ܾ� ����: " << count << endl;
}

// �� �� ��ü�� �Ųٷ� ����ϱ�
void commandD() {
    for (auto& line : currentLines) {
        reverse(line.begin(), line.end());
    }
    printCurrentLines();
}

// 3���� �� 2���� @ �����ϰ� ��� ��� �߰�
void commandE() {
    if (isModifiedE) {
        currentLines = modifiedLines;
        isModifiedE = false;
        printCurrentLines();
        return;
    }

    modifiedLines = currentLines; // ���� ���� ����
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

// ���� ���� ��� �ܾ� �Ųٷ� ���
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

// Ư�� ���ڸ� �ٸ� ���ڷ� �ٲٰ� �����·� �ǵ�����
void commandG() {
    static char lastOldChar, lastNewChar; // ���������� �ٲ� ���ڵ� ����

    if (isModifiedG) {
        for (auto& line : currentLines) {
            replace(line.begin(), line.end(), lastNewChar, lastOldChar); // ���� ���·� ����
        }
        isModifiedG = false;
        printCurrentLines();
        return;
    }

    char oldChar, newChar;
    cout << "�ٲ� ���ڿ� ���ο� ���ڸ� �Է��ϼ���: ";
    cin >> oldChar >> newChar;
    modifiedLines = currentLines; // ���� ���� ����

    for (auto& line : currentLines) {
        replace(line.begin(), line.end(), oldChar, newChar);
    }

    lastOldChar = oldChar;
    lastNewChar = newChar;
    isModifiedG = true;
    printCurrentLines();
}

// �� ���� �ܾ� ���� ���
void commandH() {
    for (const auto& line : currentLines) {
        istringstream iss(line);
        int wordCount = 0;
        string word;
        while (iss >> word) {
            wordCount++;
        }
        cout << "�ܾ� ����: " << wordCount << " - " << line << endl;
    }
}

// �ܾ� ������ ���� ����, �������� �� �������� �� ���� ����
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

// Ư�� �ܾ� ã�Ƽ� �ٸ� ������ ����ϰ� ���� ���
void commandS() {
    string searchWord;
    cout << "ã�� �ܾ �Է��ϼ���: ";
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
                cout << "\033[32m" << word << "\033[0m ";  // �ܾ �ʷϻ����� ���
                totalCount++;
            }
            else {
                cout << word << " ";
            }
        }
        cout << endl;
    }
    cout << "�ܾ� \"" << searchWord << "\"�� �� ����: " << totalCount << endl;
}

int main() {
    string fileName;
    cout << "���� �̸��� �Է��ϼ���: ";
    cin >> fileName;

    readFile(fileName);
    printCurrentLines();

    char command;
    while (true) {
        cout << "\n��ɾ �Է��ϼ��� (c~q): ";
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
            cout << "���α׷��� �����մϴ�." << endl;
            return 0;
        default:
            cout << "�ùٸ��� ���� ��ɾ��Դϴ�. �ٽ� �Է��ϼ���." << endl;
            break;
        }
    }
}
