#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include"include/btBulletDynamicsCommon.h"

struct Vertex {
    float x, y, z;
    Vertex() = default;
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

    glm::vec3 toVec3() const { return glm::vec3(x, y, z); }
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
    std::vector<unsigned int> normalFaces; // Face에 맞는 법선 인덱스


    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::mat4(1.0f);  // 모델이 이동한 위치를 저장할 벡터
    glm::mat4 rotateMatrix = glm::mat4(1.0f);    // 모델의 초기 회전 행렬

    Material material;
    std::vector<glm::vec3> colors;      // **각 정점에 대한 색상 배열**

    std::string name;
    std::string type; // 모델 도형 타입 ex) box, sphere, cylinder

    GLuint textureID;  // 텍스처 ID 필드 추가

    bool model_status = true;
    btRigidBody* rigidBody = nullptr;  // 각 모델의 강체
    bool rigid_status = true;
};