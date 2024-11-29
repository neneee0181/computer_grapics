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
            // VAO 바인딩
            glBindVertexArray(this->vao);
            glLineWidth(1.0f);

            // 텍스처가 있는 경우
            if (this->material.hasTexture) {
                // Environment Texture (map_Ka)
                if (material.ambientTextureID != 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, material.ambientTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Ka"), 0);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKaTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKaTexture"), 0);
                }

                // Diffuse Texture (map_Kd)
                if (material.diffuseTextureID != 0) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, material.diffuseTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Kd"), 1);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKdTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKdTexture"), 0);
                }

                // Specular Texture (map_Ks)
                if (material.specularTextureID != 0) {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, material.specularTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Ks"), 2);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKsTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKsTexture"), 0);
                }
            }
            // 텍스처가 없는 경우 기본 재질 속성만 사용
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                // 기본 재질 속성 전달
                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(this->material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(this->material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(this->material.Ks));
                glUniform1f(NsLoc, this->material.Ns);
            }

            // 모델 매트릭스와 법선 매트릭스 전달
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform1i(modelStatus, 0);

            // 렌더링 모드 (1번 키로 와이어프레임 전환)
            if (isKeyPressed_s('1')) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // 삼각형 렌더링
            glDrawElements(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, 0);

            // OpenGL 상태 초기화
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 렌더링 모드 초기화

            // 텍스처 상태 초기화
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

            // VAO 바인딩 해제
            
            
            
            (0);
        }
    }


    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // 충돌 박스 그리기
        }
    }

    void initBuffer() override {

        for (const Face& face : this->faces) {
            std::cout << "Face Indices: "
                << "v1=" << face.v1 << ", v2=" << face.v2 << ", v3=" << face.v3 << " | "
                << "t1=" << face.t1 << ", t2=" << face.t2 << ", t3=" << face.t3 << std::endl;

            std::cout << "Texture Coords: "
                << "t1=(" << this->texCoords[face.t1].u << ", " << this->texCoords[face.t1].v << "), "
                << "t2=(" << this->texCoords[face.t2].u << ", " << this->texCoords[face.t2].v << "), "
                << "t3=(" << this->texCoords[face.t3].u << ", " << this->texCoords[face.t3].v << ")"
                << std::endl;
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO 생성
        glGenBuffers(4, vbos);

        // **정점 버퍼 설정**
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // location 0에 정점 할당
        glEnableVertexAttribArray(0);

        // **법선 버퍼 재구성 및 설정**
        std::vector<glm::vec3> restructuredNormals;
        for (const Face& face : this->faces) {
            // 각 Face의 법선 인덱스를 사용하여 glm::vec3로 변환 후 추가
            restructuredNormals.push_back(glm::vec3(this->normals[face.n1].nx, this->normals[face.n1].ny, this->normals[face.n1].nz));
            restructuredNormals.push_back(glm::vec3(this->normals[face.n2].nx, this->normals[face.n2].ny, this->normals[face.n2].nz));
            restructuredNormals.push_back(glm::vec3(this->normals[face.n3].nx, this->normals[face.n3].ny, this->normals[face.n3].nz));
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, restructuredNormals.size() * sizeof(glm::vec3), restructuredNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // location 1에 법선 할당
        glEnableVertexAttribArray(1);

        // **텍스처 좌표 재구성 및 설정**
        std::vector<glm::vec2> faceTexCoords;
        if (!this->texCoords.empty()) {
            for (const Face& face : this->faces) {
                faceTexCoords.push_back(glm::vec2(this->texCoords[face.t1].u, this->texCoords[face.t1].v));
                faceTexCoords.push_back(glm::vec2(this->texCoords[face.t2].u, this->texCoords[face.t2].v));
                faceTexCoords.push_back(glm::vec2(this->texCoords[face.t3].u, this->texCoords[face.t3].v));
            }

            glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
            glBufferData(GL_ARRAY_BUFFER, faceTexCoords.size() * sizeof(glm::vec2), faceTexCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // location 2에 텍스처 좌표 할당
            glEnableVertexAttribArray(2);
        }

        // **인덱스 버퍼 설정 (EBO)**
        std::vector<unsigned int> indices;
        for (const Face& face : this->faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // VAO 바인딩 해제
        glBindVertexArray(0);
    }
};