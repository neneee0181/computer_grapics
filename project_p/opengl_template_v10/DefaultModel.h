#pragma once

#include <vector>
#include <string>
#include <gl/glew.h>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"
#include "LoadObj.h"
#include "BulletPhysics.h"

using namespace std;

class DefaultModel : public Model {
public:
    DefaultModel(){}

    DefaultModel(string name, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) {
        read_obj_file(name, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
    }

    ~DefaultModel(){}

    void load_obj(string name, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override {
        read_obj_file(name, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
    }

    const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override {

        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint normalLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");

        if (this->model_status) {
            glBindVertexArray(this->vao);
            glLineWidth(1.0f);

            // Face를 하나씩 처리
            cout << faces.size() << endl;
            for (size_t faceIndex = 0; faceIndex < this->faces.size(); ++faceIndex) {
                const Face& face = this->faces[faceIndex];
                int materialIndex = face.materialIndex;

                // 유효한 Material이 있는 경우 처리
                if (materialIndex >= 0 && materialIndex < this->materials.size()) {
                    const Material& material = this->materials[materialIndex];

                    // 텍스처가 있는 경우
                    if (material.hasTexture) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, material.textureID);
                        glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
                        glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
                    }
                    // 텍스처가 없는 경우 (기본 색상 사용)
                    else {
                        glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                        GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                        GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                        GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                        GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                        glUniform3fv(KaLoc, 1, glm::value_ptr(material.Ka));
                        glUniform3fv(KdLoc, 1, glm::value_ptr(material.Kd));
                        glUniform3fv(KsLoc, 1, glm::value_ptr(material.Ks));
                        glUniform1f(NsLoc, material.Ns);
                    }
                }

               

                // 모델 매트릭스와 법선 매트릭스 전달
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                // 폴리곤 모드 설정
                glUniform1i(modelStatus, 0);
                if (isKeyPressed_s('1'))
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                // Face를 렌더링 (현재 Face만)
                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    3, // 삼각형당 3개의 인덱스
                    GL_UNSIGNED_INT,
                    (void*)(faceIndex * 3 * sizeof(unsigned int)),
                    0
                );
            }

            // 렌더링 이후 기본값으로 초기화
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

            glBindVertexArray(0);
        }
    }


    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // 충돌 박스 그리기
        }
    }

    void initBuffer() override {
        glGenVertexArrays(1, &vao); // VAO 생성
        glBindVertexArray(vao);

        // VBO 생성
        glGenBuffers(4, vbos);

        // 1. 정점 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // location 0에 정점 할당
        glEnableVertexAttribArray(0);

        // 2. 법선 데이터를 normalFaces 기반으로 재구성
        vector<glm::vec3> restructuredNormals;
        for (size_t j = 0; j < this->normalFaces.size(); ++j) {
            unsigned int normalIndex = this->normalFaces[j];
            if (normalIndex < this->normals.size()) {
                const Normal& normal = this->normals[normalIndex];
                restructuredNormals.push_back(glm::vec3(normal.nx, normal.ny, normal.nz));
            }
        }

        // 법선 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, restructuredNormals.size() * sizeof(glm::vec3), restructuredNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // location 1에 법선 할당
        glEnableVertexAttribArray(1);

        // 3. 텍스처 좌표 VBO 설정
        vector<glm::vec2> faceTexCoords;
        if (!this->texCoords.empty()) {
            for (const Face& face : this->faces) {
                // 디버깅 로그 추가
                std::cout << "face.t1: " << face.t1 << ", face.t2: " << face.t2 << ", face.t3: " << face.t3 << std::endl;
                std::cout << "texCoords.size(): " << this->texCoords.size() << std::endl;

                if (face.t1 < this->texCoords.size() && face.t2 < this->texCoords.size() && face.t3 < this->texCoords.size()) {
                    faceTexCoords.push_back(glm::vec2(this->texCoords[face.t1].u, this->texCoords[face.t1].v));
                    faceTexCoords.push_back(glm::vec2(this->texCoords[face.t2].u, this->texCoords[face.t2].v));
                    faceTexCoords.push_back(glm::vec2(this->texCoords[face.t3].u, this->texCoords[face.t3].v));
                }
                else {
                    std::cerr << "ERROR: Texture coordinate index out of range for face: "
                        << &face - &this->faces[0] << std::endl;
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, vbos[2]); // 텍스처 좌표용 VBO
            glBufferData(GL_ARRAY_BUFFER, faceTexCoords.size() * sizeof(glm::vec2), faceTexCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // location 2에 텍스처 좌표 할당
            glEnableVertexAttribArray(2);
        }

        // 4. 면 인덱스 데이터 (EBO) 설정
        vector<unsigned int> indices;
        for (const Face& face : this->faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // 5. 텍스처 이미지 활성화 및 설정
        for (const auto& material : this->materials) {
            if (material.hasTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material.textureID);

                // 텍스처 매핑 방식 설정 (선형 필터링과 반복 설정)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }

        glBindVertexArray(0); // VAO 바인딩 해제
    }
};