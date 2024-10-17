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
    unsigned int v1, v2, v3;  // 정점 인덱스
    unsigned int t1, t2, t3;  // 텍스처 좌표 인덱스 (선택적)
    unsigned int n1, n2, n3;  // 법선 벡터 인덱스
};

struct Model {
    std::vector<Vertex> vertices;  // 정점 배열
    std::vector<TextureCoord> texCoords;  // 텍스처 좌표 배열 (추가)
    std::vector<Normal> normals;   // 법선 벡터 배열
    std::vector<Face> faces;       // 면 배열
};

// OBJ 파일을 읽어와서 모델 데이터를 파싱하는 함수
void read_obj_file(const std::string& filename, Model& model) {
    std::ifstream file(filename);  // 파일 읽기 모드로 열기
    if (!file.is_open()) {  // 파일을 열지 못한 경우
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::string line;
    // 파일을 한 줄씩 읽어가며 정점, 텍스처 좌표, 법선 벡터, 면 데이터를 처리하는 루프
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // 정점 데이터를 읽을 때
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            model.vertices.push_back(vertex);
        }
        else if (prefix == "vt") {  // 텍스처 좌표 데이터를 읽을 때
            TextureCoord texCoord;
            ss >> texCoord.u >> texCoord.v;
            model.texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {  // 법선 벡터 데이터를 읽을 때
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            model.normals.push_back(normal);
        }
        else if (prefix == "f") {  // 면 데이터를 읽을 때
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
            unsigned int v, t = 0, n;
            char slash;

            while (ss >> v) {
                vertexIndices.push_back(v - 1); // OBJ 인덱스는 1부터 시작하므로 -1
                if (ss.peek() == '/') {
                    ss >> slash;
                    if (ss.peek() != '/') {
                        ss >> t;
                        texCoordIndices.push_back(t - 1);
                    }
                    else {
                        texCoordIndices.push_back(0);  // 텍스처 좌표가 없을 경우
                    }
                    if (ss.peek() == '/') {
                        ss >> slash >> n;
                        normalIndices.push_back(n - 1);
                    }
                }
            }

            // 삼각형 또는 사각형인지 확인
            if (vertexIndices.size() == 3) {
                // 삼각형인 경우
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
                // 사각형인 경우, 두 개의 삼각형으로 분할
                Face face1, face2;

                // 첫 번째 삼각형 (v1, v2, v3)
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

                // 두 번째 삼각형 (v1, v3, v4)
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
    }

    file.close();  // 파일 닫기
}
