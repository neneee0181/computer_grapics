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

            // Face�� �ϳ��� ó��
            cout << faces.size() << endl;
            for (size_t faceIndex = 0; faceIndex < this->faces.size(); ++faceIndex) {
                const Face& face = this->faces[faceIndex];
                int materialIndex = face.materialIndex;

                // ��ȿ�� Material�� �ִ� ��� ó��
                if (materialIndex >= 0 && materialIndex < this->materials.size()) {
                    const Material& material = this->materials[materialIndex];

                    // �ؽ�ó�� �ִ� ���
                    if (material.hasTexture) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, material.textureID);
                        glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
                        glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
                    }
                    // �ؽ�ó�� ���� ��� (�⺻ ���� ���)
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

               

                // �� ��Ʈ������ ���� ��Ʈ���� ����
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                // ������ ��� ����
                glUniform1i(modelStatus, 0);
                if (isKeyPressed_s('1'))
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                // Face�� ������ (���� Face��)
                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    3, // �ﰢ���� 3���� �ε���
                    GL_UNSIGNED_INT,
                    (void*)(faceIndex * 3 * sizeof(unsigned int)),
                    0
                );
            }

            // ������ ���� �⺻������ �ʱ�ȭ
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

            glBindVertexArray(0);
        }
    }


    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // �浹 �ڽ� �׸���
        }
    }

    void initBuffer() override {
        glGenVertexArrays(1, &vao); // VAO ����
        glBindVertexArray(vao);

        // VBO ����
        glGenBuffers(4, vbos);

        // 1. ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // location 0�� ���� �Ҵ�
        glEnableVertexAttribArray(0);

        // 2. ���� �����͸� normalFaces ������� �籸��
        vector<glm::vec3> restructuredNormals;
        for (size_t j = 0; j < this->normalFaces.size(); ++j) {
            unsigned int normalIndex = this->normalFaces[j];
            if (normalIndex < this->normals.size()) {
                const Normal& normal = this->normals[normalIndex];
                restructuredNormals.push_back(glm::vec3(normal.nx, normal.ny, normal.nz));
            }
        }

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, restructuredNormals.size() * sizeof(glm::vec3), restructuredNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // location 1�� ���� �Ҵ�
        glEnableVertexAttribArray(1);

        // 3. �ؽ�ó ��ǥ VBO ����
        vector<glm::vec2> faceTexCoords;
        if (!this->texCoords.empty()) {
            for (const Face& face : this->faces) {
                // ����� �α� �߰�
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

            glBindBuffer(GL_ARRAY_BUFFER, vbos[2]); // �ؽ�ó ��ǥ�� VBO
            glBufferData(GL_ARRAY_BUFFER, faceTexCoords.size() * sizeof(glm::vec2), faceTexCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // location 2�� �ؽ�ó ��ǥ �Ҵ�
            glEnableVertexAttribArray(2);
        }

        // 4. �� �ε��� ������ (EBO) ����
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

        // 5. �ؽ�ó �̹��� Ȱ��ȭ �� ����
        for (const auto& material : this->materials) {
            if (material.hasTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material.textureID);

                // �ؽ�ó ���� ��� ���� (���� ���͸��� �ݺ� ����)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }

        glBindVertexArray(0); // VAO ���ε� ����
    }
};