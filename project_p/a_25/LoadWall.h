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

namespace SQU{

    // 벽
    vector<Model> models;
    vector<GLuint> vaos;
    vector<vector<GLuint>> vbos;

    void load_obj() {

        Model model_1, model_2, model_sphere;
        read_obj_file("obj/big_box.obj", model_1, "box", "box");
        read_obj_file("obj/big_box.obj", model_2, "box", "box");
        read_obj_file("obj/sphere1.obj", model_sphere, "sphere", "box");

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, 0.0));
        matrix = glm::scale(matrix, glm::vec3(1.0, 1.0, 1.0));
        model_1.matrix = matrix * model_1.matrix;
        model_1.material.Ka = glm::vec3(1.0f, 0.5f, 0.5f);
        models.push_back(model_1);

        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(15.0, 0.0, 0.0));
        //matrix = glm::scale(matrix, glm::vec3(25.0, 25.0, 25.0));
        model_sphere.matrix = matrix * model_sphere.matrix;
        model_sphere.material.Ka = glm::vec3(0.0f, 0.0f, 1.0f);

        models.push_back(model_sphere);

        for (auto& model : models) {
            if (!model.material.map_Kd.empty()) {
                model.material.textureID = loadTexture(model.material.map_Kd);
                model.material.hasTexture = true; // 텍스처가 있음을 표시
            }
            else {
                model.material.hasTexture = false;
            }
        }
    }

    void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) {

        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint normalLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");

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


                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].matrix));
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(models[i].matrix)));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

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

            // **법선 데이터를 normalFaces 기반으로 재구성**
            vector<glm::vec3> restructuredNormals; // Face에 맞는 법선을 저장할 벡터
            for (size_t j = 0; j < models[i].normalFaces.size(); ++j) {
                unsigned int normalIndex = models[i].normalFaces[j]; // normalFaces에서 법선 인덱스 가져오기
                const Normal& normal = models[i].normals[normalIndex]; // 인덱스에 해당하는 법선 가져오기
                restructuredNormals.push_back(glm::vec3(normal.nx, normal.ny, normal.nz)); // glm::vec3로 변환 후 추가
            }

            // 법선 버퍼 설정
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // 법선용 VBO
            glBufferData(GL_ARRAY_BUFFER, restructuredNormals.size() * sizeof(glm::vec3), restructuredNormals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
            glEnableVertexAttribArray(1);

            // 텍스처 좌표 VBO 설정
            //vector<glm::vec2> faceTexCoords; // Face에 맞는 텍스처 좌표를 저장
            //if (!models[i].texCoords.empty()) {
            //    for (const Face& face : models[i].faces) {
            //        faceTexCoords.push_back(glm::vec2(
            //            models[i].texCoords[face.t1].u,
            //            models[i].texCoords[face.t1].v
            //        ));
            //        faceTexCoords.push_back(glm::vec2(
            //            models[i].texCoords[face.t2].u,
            //            models[i].texCoords[face.t2].v
            //        ));
            //        faceTexCoords.push_back(glm::vec2(
            //            models[i].texCoords[face.t3].u,
            //            models[i].texCoords[face.t3].v
            //        ));
            //    }

            //    glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);  // 텍스처 좌표용 VBO
            //    glBufferData(GL_ARRAY_BUFFER, faceTexCoords.size() * sizeof(glm::vec2), faceTexCoords.data(), GL_STATIC_DRAW);
            //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
            //    glEnableVertexAttribArray(2);
            //}

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

            glBindVertexArray(0); // VAO 바인딩 해제
        }
    }
}
