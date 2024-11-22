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

namespace Body {

    vector<Model> models;
    vector<GLuint> vaos;
    vector<vector<GLuint>> vbos;

    void load_obj() {

        Model model_body, model_box;
        read_obj_file("obj/body.obj", model_body, "body", "box");
        read_obj_file("obj/box1.obj", model_box, "box", "box");

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0, -15.2, 0.0));
        matrix = glm::scale(matrix, glm::vec3(0.6, 0.6, 0.6));
        model_body.modelMatrix = matrix * model_body.modelMatrix;
        models.push_back(model_body);

        //왼쪽 발
        matrix = glm::mat4(1.0f);
        Model model_left_leg = model_box;
        matrix = glm::translate(matrix, glm::vec3(-2.0, -23.0, 0.0));
        matrix = glm::scale(matrix, glm::vec3(1.5, 1.9, 1.5));
        model_left_leg.modelMatrix = matrix * model_left_leg.modelMatrix;
        model_left_leg.name = "left_leg";
        models.push_back(model_left_leg);

        //오른쪽 발
        matrix = glm::mat4(1.0f);
        Model model_right_leg = model_box;
        matrix = glm::translate(matrix, glm::vec3(2.0, -23.0, 0.0));
        matrix = glm::scale(matrix, glm::vec3(1.5, 1.9, 1.5));
        model_right_leg.modelMatrix = matrix * model_right_leg.modelMatrix;
        model_right_leg.name = "right_leg";
        models.push_back(model_right_leg);

        //왼쪽 팔
        matrix = glm::mat4(1.0f);
        Model model_left_arm = model_box;
        matrix = glm::translate(matrix, glm::vec3(-4.0, -16.0, 0.0));
        matrix = glm::scale(matrix, glm::vec3(1.5, 1.9, 1.5));
        matrix = glm::rotate(matrix, glm::radians(-45.0f), glm::vec3(0.0, 0.0, 1.0));
        model_left_arm.modelMatrix = matrix * model_left_arm.modelMatrix;
        model_left_arm.name = "left_arm";
        models.push_back(model_left_arm);

        //왼쪽 팔
        matrix = glm::mat4(1.0f);
        Model model_right_arm = model_box;
        matrix = glm::translate(matrix, glm::vec3(4.0, -16.0, 0.0));
        matrix = glm::scale(matrix, glm::vec3(1.5, 1.9, 1.5));
        matrix = glm::rotate(matrix, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
        model_right_arm.modelMatrix = matrix * model_right_arm.modelMatrix;
        model_right_arm.name = "right_arm";
        models.push_back(model_right_arm);


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

    void draw(GLuint shaderProgramID, bool (*isKeyPressed_s)(const char&), glm::mat4 bodyRo) {

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

                
                if (models[i].name == "body")
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix * bodyRo));
                else {
                    glm::mat4 matrix = glm::mat4(1.0f);
                    matrix = glm::translate(matrix, glm::vec3(models[0].modelMatrix[3]));
                    matrix = matrix * bodyRo;
                    matrix = glm::translate(matrix, glm::vec3(-models[0].modelMatrix[3]));
                    matrix = matrix * models[i].modelMatrix;

                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
                }
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
