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

            // �ؽ�ó�� �ִ� ���
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
            // �ؽ�ó�� ���� ��� �⺻ ���� �Ӽ��� ���
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                // �⺻ ���� �Ӽ� ����
                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(this->material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(this->material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(this->material.Ks));
                glUniform1f(NsLoc, this->material.Ns);
            }

            // �� ��Ʈ������ ���� ��Ʈ���� ����
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform1i(modelStatus, 0);

            // ������ ��� (1�� Ű�� ���̾������� ��ȯ)
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // �ﰢ�� ������
            glDrawElements(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, 0);

            // ������ ���� �⺻������ �ʱ�ȭ
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

            // VAO �� �ؽ�ó ���ε� ����
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }


    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // �浹 �ڽ� �׸���
        }
    }

    void initBuffer() override {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO ����
        glGenBuffers(4, vbos);

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0�� ���� �Ҵ�
        glEnableVertexAttribArray(0);

        // **���� �����͸� normalFaces ������� �籸��**
        vector<glm::vec3> restructuredNormals; // Face�� �´� ������ ������ ����
        for (size_t j = 0; j < this->normalFaces.size(); ++j) {
            unsigned int normalIndex = this->normalFaces[j]; // normalFaces���� ���� �ε��� ��������
            const Normal& normal = this->normals[normalIndex]; // �ε����� �ش��ϴ� ���� ��������
            restructuredNormals.push_back(glm::vec3(normal.nx, normal.ny, normal.nz)); // glm::vec3�� ��ȯ �� �߰�
        }

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);  // ������ VBO
        glBufferData(GL_ARRAY_BUFFER, restructuredNormals.size() * sizeof(glm::vec3), restructuredNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
        glEnableVertexAttribArray(1);

        // �ؽ�ó ��ǥ VBO ����
        vector<glm::vec2> faceTexCoords; // Face�� �´� �ؽ�ó ��ǥ�� ����
        if (!this->texCoords.empty()) {
            for (const Face& face : this->faces) {
                faceTexCoords.push_back(glm::vec2(
                    this->texCoords[face.t1].u,
                    this->texCoords[face.t1].v
                ));
                faceTexCoords.push_back(glm::vec2(
                    this->texCoords[face.t2].u,
                    this->texCoords[face.t2].v
                ));
                faceTexCoords.push_back(glm::vec2(
                    this->texCoords[face.t3].u,
                    this->texCoords[face.t3].v
                ));
            }

            glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);  // �ؽ�ó ��ǥ�� VBO
            glBufferData(GL_ARRAY_BUFFER, faceTexCoords.size() * sizeof(glm::vec2), faceTexCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2�� �ؽ�ó ��ǥ �Ҵ�
            glEnableVertexAttribArray(2);
        }

        // �� �ε��� ������ (EBO) ����
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

        glBindVertexArray(0); // VAO ���ε� ����

    }
};