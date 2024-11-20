#pragma once

#include <vector>
#include <string>
#include <gl/glew.h>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"
#include "LoadObj.h"

using namespace std;

// 벽
vector<Model> wall_models;
vector<GLuint> wall_vaos;
vector<vector<GLuint>> wall_vbos;


void load_wall_obj() {

    Model model_box;
    read_obj_file("obj/big_box.obj", model_box, "box");
    //read_obj_file("obj/body.obj", model_body, "body");

    glm::mat4 matrix_box = glm::mat4(1.0f);
    matrix_box = glm::translate(matrix_box, glm::vec3(0.0, 0.0, 0.0));
    model_box.modelMatrix = matrix_box * model_box.modelMatrix;

    wall_models.push_back(model_box);

    for (auto& model : wall_models) {
        if (!model.material.map_Kd.empty()) {
            model.material.textureID = loadTexture(model.material.map_Kd);
            model.material.hasTexture = true; // 텍스처가 있음을 표시
        }
        else {
            model.material.hasTexture = false;
        }
    }
}

void draw_wall(GLuint& shaderProgramID, bool (*isKeyPressed_s)(const char&)) {
    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < wall_models.size(); ++i) {

        if (wall_models[i].model_status) {
            glBindVertexArray(wall_vaos[i]);
            glLineWidth(1.0f);
            if (wall_models[i].material.hasTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, wall_models[i].material.textureID);
                glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(wall_models[i].material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(wall_models[i].material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(wall_models[i].material.Ks));
                glUniform1f(NsLoc, wall_models[i].material.Ns);
            }

            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall_models[i].modelMatrix));
            glUniform1i(modelStatus, 0);
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, wall_models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
        }
    }
    glDisable(GL_DEPTH_TEST);
}

void initBuffer_wall() {
    wall_vaos.resize(wall_models.size());
    wall_vbos.resize(wall_models.size(), vector<GLuint>(4)); // 모델마다 4개의 VBO가 필요 (정점, 색상, 법선, 텍스처 좌표)

    for (size_t i = 0; i < wall_models.size(); ++i) {
        glGenVertexArrays(1, &wall_vaos[i]);
        glBindVertexArray(wall_vaos[i]);

        // VBO 생성
        glGenBuffers(4, wall_vbos[i].data());

        // 정점 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, wall_vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, wall_models[i].vertices.size() * sizeof(glm::vec3), wall_models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
        glEnableVertexAttribArray(0);

        // 색상 버퍼 설정
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 색상 할당
        //glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, wall_vbos[i][2]);  // 법선용 VBO
        glBufferData(GL_ARRAY_BUFFER, wall_models[i].normals.size() * sizeof(glm::vec3), wall_models[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
        glEnableVertexAttribArray(1);

        // 텍스처 좌표 VBO 설정
        glBindBuffer(GL_ARRAY_BUFFER, wall_vbos[i][3]);  // 4번째 VBO가 텍스처 좌표용이라고 가정
        glBufferData(GL_ARRAY_BUFFER, wall_models[i].texCoords.size() * sizeof(glm::vec2), wall_models[i].texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
        glEnableVertexAttribArray(2);

        // 면 인덱스 데이터 (EBO) 설정
        vector<unsigned int> indices;
        for (const Face& face : wall_models[i].faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}