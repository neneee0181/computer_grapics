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
    unsigned int v1, v2, v3;  // ���� �ε���
    unsigned int t1, t2, t3;  // �ؽ�ó ��ǥ �ε��� (������)
    unsigned int n1, n2, n3;  // ���� ���� �ε���
};

struct Material {
    glm::vec3 Ka;  // ȯ�汤 ����
    glm::vec3 Kd;  // ���ݻ籤 ����
    glm::vec3 Ks;  // �ݻ籤 ����
    float Ns;      // ��¦�� ����
    std::string map_Kd;  // �ؽ�ó �̹��� ��� (������)
    GLuint textureID;  // �ؽ�ó ID �ʵ� �߰�
    bool hasTexture;  // �ؽ�ó�� �ִ��� ����
};

struct Model {
    std::vector<Vertex> vertices;  // ���� �迭
    std::vector<TextureCoord> texCoords;  // �ؽ�ó ��ǥ �迭 (�߰�)
    std::vector<Normal> normals;   // ���� ���� �迭
    std::vector<Face> faces;       // �� �迭

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::mat4(1.0f);  // ���� �̵��� ��ġ�� ������ ����
    glm::mat4 rotateMatrix = glm::mat4(1.0f);    // ���� �ʱ� ȸ�� ���

    Material material;
    std::vector<glm::vec3> colors;      // **�� ������ ���� ���� �迭**

    std::string name;

    GLuint textureID;  // �ؽ�ó ID �ʵ� �߰�

    bool model_status = true;
    btRigidBody* rigidBody = nullptr;  // �� ���� ��ü

};