#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "LoadMtl.h"
#include "tiny_obj_loader.h"

// tinyobjloader�� �̿��Ͽ� OBJ ������ �д� �Լ�
void read_obj_file(const std::string& filename, Model* model, const std::string& name, const std::string& type) {
    tinyobj::attrib_t attrib; // ����, �ؽ�ó ��ǥ, ���� �����͸� �����ϴ� ����ü
    std::vector<tinyobj::shape_t> shapes; // ����(Shape) ����Ʈ
    std::vector<tinyobj::material_t> materials; // ����(Material) ����Ʈ
    std::string warn, err; // ���� ���� �޽��� ����

    // tinyobjloader�� OBJ ���� �ε�
    bool ret = tinyobj::LoadObj(
        &attrib,       // ����, �ؽ�ó ��ǥ, ���� �����Ͱ� �����
        &shapes,       // ����(face) �����Ͱ� �����
        &materials,    // ����(material) �����Ͱ� �����
        &warn,         // ��� �޽���
        &err,          // ���� �޽���
        filename.c_str(), // OBJ ���� ���
        nullptr          // MTL ������ �⺻ ���(NULL�̸� OBJ ���ϰ� ���� ��ο��� MTL �ε�)
    );

    // ��� �޽��� ���
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    // ���� �޽��� ��� �� ���� ó��
    if (!err.empty()) {
        std::cerr << "ERROR: " << err << std::endl;
        if (!ret) {
            return;
        }
    }

    // �� �̸� �� Ÿ�� ����
    model->name = name;
    model->type = type;

    // **���� ������ �߰�**
    for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
        Vertex vertex;
        vertex.x = attrib.vertices[3 * i + 0];
        vertex.y = attrib.vertices[3 * i + 1];
        vertex.z = attrib.vertices[3 * i + 2];
        model->vertices.push_back(vertex);
    }

    // **�ؽ�ó ��ǥ ������ �߰�**
    for (size_t i = 0; i < attrib.texcoords.size() / 2; i++) {
        TextureCoord texCoord;
        texCoord.u = attrib.texcoords[2 * i + 0];
        texCoord.v = attrib.texcoords[2 * i + 1];
        model->texCoords.push_back(texCoord);
    }

    // **���� ������ �߰�**
    for (size_t i = 0; i < attrib.normals.size() / 3; i++) {
        Normal normal;
        normal.nx = attrib.normals[3 * i + 0];
        normal.ny = attrib.normals[3 * i + 1];
        normal.nz = attrib.normals[3 * i + 2];
        model->normals.push_back(normal);
    }

    // **���� ������ (faces) �߰�**
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f]; // Face�� ���� ���� (3 �Ǵ� 4)

            // Face ������ �ʱ�ȭ
            Face face;
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;

            // Face�� Material Index ����
            if (shape.mesh.material_ids[f] >= 0) {
                face.materialIndex = shape.mesh.material_ids[f];
            }
            else {
                face.materialIndex = -1; // Material�� ���� ��� �⺻�� -1
            }

            // Face�� �� ���� ó��
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // ���� �ε��� �߰�
                vertexIndices.push_back(idx.vertex_index);

                // �ؽ�ó ��ǥ �ε��� �߰�
                if (idx.texcoord_index >= 0) {
                    texCoordIndices.push_back(idx.texcoord_index);
                }
                else {
                    texCoordIndices.push_back(0); // �ؽ�ó ��ǥ�� ���� ��� �⺻��
                }

                // ���� �ε��� �߰�
                if (idx.normal_index >= 0) {
                    normalIndices.push_back(idx.normal_index);
                }
                else {
                    normalIndices.push_back(0); // ������ ���� ��� �⺻��
                }
            }

            // �ﰢ�� Face ó��
            if (fv == 3) {
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];

                face.t1 = texCoordIndices[0];
                face.t2 = texCoordIndices[1];
                face.t3 = texCoordIndices[2];

                face.n1 = normalIndices[0];
                face.n2 = normalIndices[1];
                face.n3 = normalIndices[2];

                model->faces.push_back(face);
            }
            // �簢�� Face ó�� (�� ���� �ﰢ������ ����)
            else if (fv == 4) {
                // ù ��° �ﰢ��
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];

                face.t1 = texCoordIndices[0];
                face.t2 = texCoordIndices[1];
                face.t3 = texCoordIndices[2];

                face.n1 = normalIndices[0];
                face.n2 = normalIndices[1];
                face.n3 = normalIndices[2];

                model->faces.push_back(face);

                // �� ��° �ﰢ��
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[2];
                face.v3 = vertexIndices[3];

                face.t1 = texCoordIndices[0];
                face.t2 = texCoordIndices[2];
                face.t3 = texCoordIndices[3];

                face.n1 = normalIndices[0];
                face.n2 = normalIndices[2];
                face.n3 = normalIndices[3];

                model->faces.push_back(face);
            }

            index_offset += fv; // Face ���� ����ŭ �ε��� ������ ����
        }
    }


    // **����(Material) ó��**
    if (!materials.empty()) {
        for (const auto& material : materials) {
            Material mat;

            mat.Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
            mat.Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            mat.Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
            mat.Ns = material.shininess;
            mat.map_Kd = material.diffuse_texname;

            // �ؽ�ó ���� Ȯ�� �� �ʱ�ȭ
            mat.hasTexture = !mat.map_Kd.empty();
            mat.textureID = 0; // �ؽ�ó ID�� ���� OpenGL���� �ε� �� ����

            model->materials.push_back(mat);
        }
    }
}