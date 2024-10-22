#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    unsigned int v1, v2, v3;  // ���� �ε���
    unsigned int t1, t2, t3;  // �ؽ�ó ��ǥ �ε��� (������)
    unsigned int n1, n2, n3;  // ���� ���� �ε���
};

struct Model {
    std::vector<Vertex> vertices;  // ���� �迭
    std::vector<TextureCoord> texCoords;  // �ؽ�ó ��ǥ �迭 (�߰�)
    std::vector<Normal> normals;   // ���� ���� �迭
    std::vector<Face> faces;       // �� �迭
};

// OBJ ������ �о�ͼ� �� �����͸� �Ľ��ϴ� �Լ�
void read_obj_file(const std::string& filename, Model& model) {
    std::ifstream file(filename);  // ���� �б� ���� ����
    if (!file.is_open()) {  // ������ ���� ���� ���
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::string line;
    // ������ �� �پ� �о�� ����, �ؽ�ó ��ǥ, ���� ����, �� �����͸� ó���ϴ� ����
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // ���� �����͸� ���� ��
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            model.vertices.push_back(vertex);
        }
        else if (prefix == "vt") {  // �ؽ�ó ��ǥ �����͸� ���� ��
            TextureCoord texCoord;
            ss >> texCoord.u >> texCoord.v;
            model.texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {  // ���� ���� �����͸� ���� ��
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            model.normals.push_back(normal);
        }
        else if (prefix == "f") {  // �� �����͸� ���� ��
            Face face;
            unsigned int v1, v2, v3;
            unsigned int t1 = 0, t2 = 0, t3 = 0;
            unsigned int n1, n2, n3;
            char slash;  // '/' �����ڸ� ó���ϱ� ���� ����

            // �ؽ�ó ��ǥ�� �ִ� ���� ���� ��츦 �����ؼ� �Ľ�
            if (line.find("//") != std::string::npos) {
                // �ؽ�ó ��ǥ�� ���� ��� v//vn
                ss >> v1 >> slash >> slash >> n1
                    >> v2 >> slash >> slash >> n2
                    >> v3 >> slash >> slash >> n3;
            }
            else {
                // �ؽ�ó ��ǥ�� �ִ� ��� v/vt/vn
                ss >> v1 >> slash >> t1 >> slash >> n1
                    >> v2 >> slash >> t2 >> slash >> n2
                    >> v3 >> slash >> t3 >> slash >> n3;
            }

            // OBJ ���Ͽ��� �ε����� 1���� �����ϹǷ�, 0���� �����ϵ��� ��ȯ
            face.v1 = v1 - 1;
            face.t1 = t1 - 1;  // �ؽ�ó ��ǥ�� ���� ��� �⺻�� 0
            face.n1 = n1 - 1;
            face.v2 = v2 - 1;
            face.t2 = t2 - 1;
            face.n2 = n2 - 1;
            face.v3 = v3 - 1;
            face.t3 = t3 - 1;
            face.n3 = n3 - 1;

            model.faces.push_back(face);
        }
    }

    file.close();  // ���� �ݱ�
}
