#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct Point {
    int x, y, z;
    bool empty = false;

    double distance() const {
        return sqrt(x * x + y * y + z * z);
    }
};

const int MAX_SIZE = 20;

vector<Point> points;
bool markedForErase = false;

void printList() {
    int k = 10;
    for (int i = 0; i < 10; ++i) {
        int index = 9 - i;
        cout << --k << ": ";
        if (index < points.size()) {
            if (points[index].empty) {
                cout << " \n";
            }
            else {
                cout << "(" << points[index].x << ", " << points[index].y << ", " << points[index].z << ")\n";
            }
        }
        else {
            cout << " \n";
        }
    }
}

void cleanUpEmptyPoints() {
    points.erase(remove_if(points.begin(), points.end(), [](const Point& p) {
        return p.empty;
        }), points.end());
}

void addTop(int x, int y, int z) {
    if (points.size() < MAX_SIZE) {
        points.insert(points.begin(), { x, y, z });
    }
    else {
        cout << "�׵���\n";
    }
}

void removeTop() {
    if (points.empty()) {
        cout << "��\n";
        return;
    }

    if (!markedForErase && !points.front().empty) {
        points.front().empty = true;
        markedForErase = true;
    }
    else {
        points.erase(points.begin());
        markedForErase = false;
    }
}

void addBottom(int x, int y, int z) {
    if (points.size() < MAX_SIZE) {
        points.push_back({ x, y, z });
    }
    else {
        cout << "����\n";
    }
}

void removeBottom() {
    if (!points.empty()) {
        points.pop_back();
    }
    else {
        cout << "��\n";
    }
}

void clearList() {
    points.clear();
    markedForErase = false;
}

void farthestPoint() {
    if (points.empty()) {
        cout << "��\n";
        return;
    }
    auto maxPoint = max_element(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.empty || (!b.empty && a.distance() < b.distance());
        });
    if (maxPoint->empty) {
        cout << "����\n";
    }
    else {
        cout << ": (" << maxPoint->x << ", " << maxPoint->y << ", " << maxPoint->z << ")\n";
    }
}

void closestPoint() {
    if (points.empty()) {
        cout << "��!\n";
        return;
    }
    auto minPoint = min_element(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.empty || (!b.empty && a.distance() < b.distance());
        });
    if (minPoint->empty) {
        cout << "����Ʈ����!\n";
    }
    else {
        cout << ": (" << minPoint->x << ", " << minPoint->y << ", " << minPoint->z << ")\n";
    }
}

void sortAscending() {
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.distance() < b.distance();
        });
}

void sortDescending() {
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.distance() > b.distance();
        });
}

void listLength() {
    cout << "����: " << points.size() << endl;
}

int main() {
    char command;
    int x, y, z;

    cout << "�Է�: ";
    while (cin >> command) {
        switch (command) {
        case '+':
            cin >> x >> y >> z;
            addBottom(x, y, z);
            break;
        case '-':
            removeBottom();
            break;
        case 'e':
            cin >> x >> y >> z;
            addTop(x, y, z);
            break;
        case 'd':
            removeTop();
            break;
        case 'l':
            listLength();
            break;
        case 'c':
            clearList();
            break;
        case 'm':
            farthestPoint();
            break;
        case 'n':
            closestPoint();
            break;
        case 'a':
            sortAscending();
            break;
        case 's':
            sortDescending();
            break;
        case 'q':
            return 0;
        default:
            cout << "�� �� ���� ��ɾ�\n";
        }

        printList();

        cleanUpEmptyPoints();
        cout << "�Է�: ";
    }
    return 0;
}
