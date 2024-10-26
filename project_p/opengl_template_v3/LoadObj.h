#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "LoadMtl.h"

// OBJ ������ �о�ͼ� �� �����͸� �Ľ��ϴ� �Լ�
void read_obj_file(const std::string& filename, Model& model, std::string name) {
    std::ifstream file(filename);  // ���� �б� ���� ����
    if (!file.is_open()) {  // ������ ���� ���� ���
        //throw std::runtime_error("Error opening file: " + filename);
        return;
    }

    std::string line;
    std::string mtlFilename;

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
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
            unsigned int v, t = 0, n;
            char slash;

            while (ss >> v) {
                vertexIndices.push_back(v - 1); // OBJ �ε����� 1���� �����ϹǷ� -1
                if (ss.peek() == '/') {
                    ss >> slash;
                    if (ss.peek() != '/') {
                        ss >> t;
                        texCoordIndices.push_back(t - 1);
                    }
                    else {
                        texCoordIndices.push_back(0);  // �ؽ�ó ��ǥ�� ���� ���
                    }
                    if (ss.peek() == '/') {
                        ss >> slash >> n;
                        normalIndices.push_back(n - 1);
                    }
                }
            }

            // �ﰢ�� �Ǵ� �簢������ Ȯ��
            if (vertexIndices.size() == 3) {
                // �ﰢ���� ���
                Face face;
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];
                face.t1 = texCoordIndices.size() > 0 ? texCoordIndices[0] : 0;
                face.t2 = texCoordIndices.size() > 1 ? texCoordIndices[1] : 0;
                face.t3 = texCoordIndices.size() > 2 ? texCoordIndices[2] : 0;
                face.n1 = normalIndices[0];
                face.n2 = normalIndices[1];
                face.n3 = normalIndices[2];
                model.faces.push_back(face);
            }
            else if (vertexIndices.size() == 4) {
                // �簢���� ���, �� ���� �ﰢ������ ����
                Face face1, face2;

                // ù ��° �ﰢ�� (v1, v2, v3)
                face1.v1 = vertexIndices[0];
                face1.v2 = vertexIndices[1];
                face1.v3 = vertexIndices[2];
                face1.t1 = texCoordIndices.size() > 0 ? texCoordIndices[0] : 0;
                face1.t2 = texCoordIndices.size() > 1 ? texCoordIndices[1] : 0;
                face1.t3 = texCoordIndices.size() > 2 ? texCoordIndices[2] : 0;
                face1.n1 = normalIndices[0];
                face1.n2 = normalIndices[1];
                face1.n3 = normalIndices[2];
                model.faces.push_back(face1);

                // �� ��° �ﰢ�� (v1, v3, v4)
                face2.v1 = vertexIndices[0];
                face2.v2 = vertexIndices[2];
                face2.v3 = vertexIndices[3];
                face2.t1 = texCoordIndices.size() > 0 ? texCoordIndices[0] : 0;
                face2.t2 = texCoordIndices.size() > 2 ? texCoordIndices[2] : 0;
                face2.t3 = texCoordIndices.size() > 3 ? texCoordIndices[3] : 0;
                face2.n1 = normalIndices[0];
                face2.n2 = normalIndices[2];
                face2.n3 = normalIndices[3];
                model.faces.push_back(face2);
            }
        }
        else if (prefix == "mtllib") {  // MTL ���� ����
            ss >> mtlFilename;
        }
    }


    model.name = name;

    if (!mtlFilename.empty()) {
        read_mtl_file(mtlFilename, model.material);  // MTL ���� �ε�
    }

    file.close();  // ���� �ݱ�
}