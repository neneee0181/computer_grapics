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
            Face face;
            unsigned int v1, v2, v3;
            unsigned int t1 = 0, t2 = 0, t3 = 0;
            unsigned int n1, n2, n3;
            char slash;  // '/' 구분자를 처리하기 위한 변수

            // 텍스처 좌표가 있는 경우와 없는 경우를 구분해서 파싱
            if (line.find("//") != std::string::npos) {
                // 텍스처 좌표가 없는 경우 v//vn
                ss >> v1 >> slash >> slash >> n1
                    >> v2 >> slash >> slash >> n2
                    >> v3 >> slash >> slash >> n3;
            }
            else {
                // 텍스처 좌표가 있는 경우 v/vt/vn
                ss >> v1 >> slash >> t1 >> slash >> n1
                    >> v2 >> slash >> t2 >> slash >> n2
                    >> v3 >> slash >> t3 >> slash >> n3;
            }

            // OBJ 파일에서 인덱스는 1부터 시작하므로, 0부터 시작하도록 변환
            face.v1 = v1 - 1;
            face.t1 = t1 - 1;  // 텍스처 좌표가 없을 경우 기본값 0
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

    file.close();  // 파일 닫기
}
