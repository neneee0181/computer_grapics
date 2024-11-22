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

namespace Wall{

    // 벽
    vector<Model> models;
    vector<GLuint> vaos;
    vector<vector<GLuint>> vbos;


    void load_obj() {

        Model model_1, model_2;
        read_obj_file("obj/plane1.obj", model_1, "plane", "box");

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, 0.0));
        model_1.modelMatrix = matrix * model_1.modelMatrix;
        model_1.material.Ka = glm::vec3(1.0f, 1.0f, 1.0f);

        model_2 = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, 0.0));
        model_2.modelMatrix = matrix * model_2.modelMatrix;
        model_2.material.Ka = glm::vec3(0.0, 0.0, 0.0f);

        Model model_left, model_right, model_back, model_top, model_front1, model_front2;
        
        //왼쪽 벽
        model_left = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(5.0, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(-5.0, 0.0, 0.0));
        matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0f));
        model_left.modelMatrix = matrix * model_left.modelMatrix;
        model_left.material.Ka = glm::vec3(0.8, 0.1, 0.1);
        model_left.name = "left";
        models.push_back(model_left);

        //우측 벽
        model_right = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(5.0, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(5.0, 0.0, 0.0));
        matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0f));
        model_right.modelMatrix = matrix * model_right.modelMatrix;
        model_right.material.Ka = glm::vec3(0.1, 0.8, 0.1);
        model_right.name = "right";
        models.push_back(model_right);

        //뒤쪽 벽
        model_back = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(5.0, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, -5.0));
        matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0f));
        model_back.modelMatrix = matrix * model_back.modelMatrix;
        model_back.material.Ka = glm::vec3(0.1, 0.1, 0.8);
        model_back.name = "back";
        models.push_back(model_back);

        //위쪽 벽
        model_top = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(5.0, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(0.0, 5.0, 0.0));
        model_top.modelMatrix = matrix * model_top.modelMatrix;
        model_top.material.Ka = glm::vec3(0.5, 0.5, 0.5);
        model_top.name = "top";
        model_top.rigid_status = false;
        models.push_back(model_top);

        model_front1 = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(2.5, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(-5.0, 0.0, 5.0));
        matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0f));
        model_front1.modelMatrix = matrix * model_front1.modelMatrix;
        model_front1.material.Ka = glm::vec3(0.8, 0.8, 0.8);
        model_front1.name = "front1";
        model_front1.rigid_status = false;
        models.push_back(model_front1);


        model_front2 = model_1;
        matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(2.5, 5.0, 5.0));
        matrix = glm::translate(matrix, glm::vec3(5.0, 0.0, 5.0));
        matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0f));
        model_front2.modelMatrix = matrix * model_front2.modelMatrix;
        model_front2.material.Ka = glm::vec3(0.8, 0.8, 0.8);
        model_front2.name = "front2";
        model_front2.rigid_status = false;
        models.push_back(model_front2);

        // 5x5 크기의 바닥 설치
        const float spacing = 10; // 각 plane의 크기 (x와 z 간 간격)
        const int gridSize = 5;

        // 중심 계산 (그리드 전체 크기의 절반만큼 이동)
        const float offset = (gridSize - 1) * spacing * 0.5f;

        for (int z = 0; z < gridSize; ++z) {
            for (int x = 0; x < gridSize; ++x) {
                Model model;

                // x와 z 위치 계산
                float posX = x * spacing - offset; // x 좌표를 중심으로 조정
                float posZ = z * spacing - offset; // z 좌표를 중심으로 조정

                // 번갈아가며 다른 재질 적용 및 위치 보정
                if ((x + z) % 2 == 0) {
                    model = model_2; // model_1은 위치 보정 없음
                }
                else {
                    model = model_1;
                }

                // 변환 행렬 생성
                glm::mat4 transform = glm::mat4(1.0f);
                transform = glm::translate(transform, glm::vec3(posX, -25.0f, posZ));
                model.modelMatrix = transform * model.modelMatrix;
                model.name = "bottom";
                //model.rigid_status = false;
                // 모델 벡터에 추가
                models.push_back(model);
            }
        }

        for (auto& model : models) {
            if (!model.material.map_Kd.empty()) {
                model.material.textureID = loadTexture(model.material.map_Kd);
                model.material.hasTexture = true; // 텍스처가 있음을 표시
            }
            else {
                model.material.hasTexture = false;
            }
            
            if (model.rigid_status) {
                addModelToPhysicsWorld(model);
            }
        }
    }

    void draw(GLuint shaderProgramID, bool (*isKeyPressed_s)(const char&)) {

        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

        for (size_t i = 0; i < models.size(); ++i) {

            if (models[i].model_status) {
                glBindVertexArray(vaos[i]);
                glLineWidth(1.0f);
                if (models[i].material.hasTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, models[i].material.textureID);
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

                    glUniform3fv(KaLoc, 1, glm::value_ptr(models[i].material.Ka));
                    glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));
                    glUniform3fv(KsLoc, 1, glm::value_ptr(models[i].material.Ks));
                    glUniform1f(NsLoc, models[i].material.Ns);
                }


                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
                glUniform1i(modelStatus, 0);
                if (isKeyPressed_s('1'))
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

                // 렌더링 이후 기본값으로 초기화
                glm::mat4 identity = glm::mat4(1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

                glBindVertexArray(0);
            }
        }
    }

    void draw_rigidBody(GLuint shaderProgramID) {
        for (const auto& model : models) {
            if (model.rigidBody) {
                RenderCollisionBox(model, shaderProgramID); // 충돌 박스 그리기
            }
        }
    }

    void initBuffer() {
        vaos.resize(models.size());
        vbos.resize(models.size(), vector<GLuint>(4)); // 모델마다 4개의 VBO가 필요 (정점, 색상, 법선, 텍스처 좌표)

        for (size_t i = 0; i < models.size(); ++i) {
            glGenVertexArrays(1, &vaos[i]);
            glBindVertexArray(vaos[i]);

            // VBO 생성
            glGenBuffers(4, vbos[i].data());

            // 정점 버퍼 설정
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
            glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
            glEnableVertexAttribArray(0);

            // 색상 버퍼 설정
            //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
            //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
            //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 색상 할당
            //glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // 법선용 VBO
            glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3), models[i].normals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
            glEnableVertexAttribArray(1);

            // 텍스처 좌표 VBO 설정
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);  // 4번째 VBO가 텍스처 좌표용이라고 가정
            glBufferData(GL_ARRAY_BUFFER, models[i].texCoords.size() * sizeof(glm::vec2), models[i].texCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
            glEnableVertexAttribArray(2);

            // 면 인덱스 데이터 (EBO) 설정
            vector<unsigned int> indices;
            for (const Face& face : models[i].faces) {
                indices.push_back(face.v1);
                indices.push_back(face.v2);
                indices.push_back(face.v3);
            }

            GLuint ebo;
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    }
}
