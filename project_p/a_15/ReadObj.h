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

struct Material {
    glm::vec3 Ka;  // 환경광 색상
    glm::vec3 Kd;  // 난반사광 색상
    glm::vec3 Ks;  // 반사광 색상
    float Ns;      // 반짝임 강도
    std::string map_Kd;  // 텍스처 이미지 경로 (선택적)
    GLuint textureID;  // 텍스처 ID 필드 추가
    bool hasTexture;  // 텍스처가 있는지 여부
};

struct Model {
    std::vector<Vertex> vertices;  // 정점 배열
    std::vector<TextureCoord> texCoords;  // 텍스처 좌표 배열 (추가)
    std::vector<Normal> normals;   // 법선 벡터 배열
    std::vector<Face> faces;       // 면 배열
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    Material material;
    std::vector<glm::vec3> colors;      // **각 정점에 대한 색상 배열**
    std::string name;

    GLuint textureID;  // 텍스처 ID 필드 추가
};


// MTL 파일을 읽어와서 재질 정보를 파싱하는 함수
void read_mtl_file(const std::string& filename, Material& material) {
    std::ifstream file(filename);  // MTL 파일 열기
    if (!file.is_open()) {  // 파일 열기 실패 시 예외 처리
        //throw std::runtime_error("Error opening MTL file: " + filename);
        return;
    }

    std::string line;
    bool hasTexture = false;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "Ka") {  // 환경광 색상
            ss >> material.Ka.x >> material.Ka.y >> material.Ka.z;
        }
        else if (prefix == "Kd") {  // 난반사광 색상
            ss >> material.Kd.x >> material.Kd.y >> material.Kd.z;
        }
        else if (prefix == "Ks") {  // 반사광 색상
            ss >> material.Ks.x >> material.Ks.y >> material.Ks.z;
        }
        else if (prefix == "Ns") {  // 반짝임 강도
            ss >> material.Ns;
        }
        else if (prefix == "map_Kd") {  // 텍스처 파일 경로
            ss >> material.map_Kd;
            hasTexture = true;
        }
    }

    material.hasTexture = hasTexture;  // 텍스처가 있는지 여부 플래그 설정
    file.close();  // 파일 닫기
}

// OBJ 파일을 읽어와서 모델 데이터를 파싱하는 함수
void read_obj_file(const std::string& filename, Model& model, std::string name) {
    std::ifstream file(filename);  // 파일 읽기 모드로 열기
    if (!file.is_open()) {  // 파일을 열지 못한 경우
        //throw std::runtime_error("Error opening file: " + filename);
        return;
    }

    std::string line;
    std::string mtlFilename;

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
        else if (prefix == "mtllib") {  // MTL 파일 참조
            ss >> mtlFilename;
        }
    }


    model.name = name;

    if (!mtlFilename.empty()) {
        read_mtl_file(mtlFilename, model.material);  // MTL 파일 로드
    }

    file.close();  // 파일 닫기
}