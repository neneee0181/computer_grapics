#pragma once

#include <vector>
#include <string>
#include <gl/glew.h>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"


class SierpinskiModel : public Model {
public:
    SierpinskiModel(int depth = 5) : depth(depth) {
        generateSierpinski(); // 시어핀스키 삼각형 데이터 생성
        //this->material.Ka = glm::vec3(0.1, 0.4, 0.7);
        this->material.Ka = glm::vec3(0.7, 0.1, 0.1);
        this->name = "sierpinski";
        this->type = "sierpinski";
        this->rigid_status = false;
    }

    ~SierpinskiModel() {
        // OpenGL 리소스 해제
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(4, vbos);
        glDeleteVertexArrays(1, &vao);
    }

    void load_obj(std::string name, std::string obj_name, std::string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override {
        // SierpinskiModel은 OBJ 로드가 필요 없으므로 이 함수는 비워둡니다.
        std::cout << "SierpinskiModel does not support OBJ loading." << std::endl;
    }

    void initBuffer() override {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO 생성 및 데이터 업로드
        glGenBuffers(4, vbos);

        // 정점 버퍼
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0 -> 정점 위치
        glEnableVertexAttribArray(0);

        // 법선 버퍼
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Normal), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1 -> 법선 벡터
        glEnableVertexAttribArray(1);

        // 텍스처 좌표 버퍼
        if (!texCoords.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
            glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(TextureCoord), texCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2 -> 텍스처 좌표
            glEnableVertexAttribArray(2);
        }

        // EBO 생성 (인덱스 버퍼)
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0); // VAO 해제
    }

    const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override {
        if (!model_status) return;

        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint normalLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");

        if (this->model_status) {
            glBindVertexArray(this->vao);
            glLineWidth(1.0f);
            if (this->material.hasTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, this->material.textureID);
                glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(this->material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(this->material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(this->material.Ks));
                glUniform1f(NsLoc, this->material.Ns);
            }

            glBindVertexArray(vao);

            // 모델 행렬 전달
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

            // 법선 변환 행렬 전달
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrix)));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // 폴리곤 모드 설정 (와이어프레임 / 채우기)
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // 렌더링 호출
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

            // 렌더링 이후 기본값으로 초기화
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

            glBindVertexArray(0); // VAO 해제
        }

    }

    const void draw_rigidBody(GLuint shaderProgramID) override {
        // 시어핀스키 삼각형의 경우 물리 충돌 박스가 필요 없으므로 구현하지 않습니다.
    }

private:
    int depth;
    std::vector<unsigned int> indices;  // 인덱스 데이터
    GLuint ebo = 0;                     // 인덱스 버퍼 (EBO)

    void generateSierpinski() {
        // 기본 삼각형 정의
        glm::vec3 a(-0.5f, -0.5f, 0.0f);
        glm::vec3 b(0.5f, -0.5f, 0.0f);
        glm::vec3 c(0.0f, 0.5f, 0.0f);

        // 재귀적으로 시어핀스키 삼각형 생성
        sierpinski(a, b, c, depth);
    }

    void sierpinski(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int depth) {
        if (depth == 0) {
            // 정점 추가
            vertices.push_back(Vertex(a.x, a.y, a.z));
            vertices.push_back(Vertex(b.x, b.y, b.z));
            vertices.push_back(Vertex(c.x, c.y, c.z));

            // 법선 계산 (삼각형 평면의 외적)
            glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
            normals.push_back(Normal{ normal.x, normal.y, normal.z });
            normals.push_back(Normal{ normal.x, normal.y, normal.z });
            normals.push_back(Normal{ normal.x, normal.y, normal.z });

            // 텍스처 좌표 추가
            texCoords.push_back(TextureCoord{ 0.0f, 0.0f }); // A
            texCoords.push_back(TextureCoord{ 1.0f, 0.0f }); // B
            texCoords.push_back(TextureCoord{ 0.5f, 1.0f }); // C

            // 인덱스 추가
            unsigned int index = vertices.size() - 3;
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            return;
        }

        // 중점 계산
        glm::vec3 mid_ab = (a + b) * 0.5f;
        glm::vec3 mid_bc = (b + c) * 0.5f;
        glm::vec3 mid_ca = (c + a) * 0.5f;

        // 재귀적으로 작은 삼각형 생성
        sierpinski(a, mid_ab, mid_ca, depth - 1);
        sierpinski(mid_ab, b, mid_bc, depth - 1);
        sierpinski(mid_ca, mid_bc, c, depth - 1);
    }
};