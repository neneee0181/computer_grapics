#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem> // C++17���� ���� ��� ó���� ���� ���

#include "Model.h"
#include "LoadMtl.h"
#include "LoadTexture.h"
#include "tiny_obj_loader.h"

// �ؽ�ó ��θ� MTL ������ ���丮 �������� ó���ϴ� �Լ�
std::string resolve_texture_path(const std::string& base_path, const std::string& texture_path) {
    // �ؽ�ó ��ΰ� ���� ������� Ȯ��
    if (std::filesystem::path(texture_path).is_absolute()) {
        return texture_path;
    }

    // �ؽ�ó ��ΰ� ��� ��ζ�� MTL ���� ��� �������� ����
    std::filesystem::path base(base_path);
    std::filesystem::path texture(texture_path);
    return (base / texture).string();
}

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
        "./obj/"          // MTL ������ �⺻ ���(NULL�̸� OBJ ���ϰ� ���� ��ο��� MTL �ε�)
    );

    // ���� �ε� ��� Ȯ��
    if (!ret) {
        std::cerr << "ERROR: Failed to load OBJ file: " << err << std::endl;
        return;
    }

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

    // ������ ũ�� Ȯ��
    std::cout << "Number of vertices: " << attrib.vertices.size() / 3 << std::endl;
    std::cout << "Number of texcoords: " << attrib.texcoords.size() / 2 << std::endl;
    std::cout << "Number of normals: " << attrib.normals.size() / 3 << std::endl;
    std::cout << "Number of shapes: " << shapes.size() << std::endl;
    std::cout << "Number of materials: " << materials.size() << std::endl;

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

    // **Face ������ �߰�**
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];
            Face face;

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // ���� �ε���
                if (v == 0) face.v1 = idx.vertex_index;
                if (v == 1) face.v2 = idx.vertex_index;
                if (v == 2) face.v3 = idx.vertex_index;

                // �ؽ�ó ��ǥ �ε���
                if (idx.texcoord_index >= 0 && idx.texcoord_index < attrib.texcoords.size() / 2) {
                    if (v == 0) face.t1 = idx.texcoord_index;
                    if (v == 1) face.t2 = idx.texcoord_index;
                    if (v == 2) face.t3 = idx.texcoord_index;
                }
                else {
                    if (v == 0) face.t1 = 0; // �⺻������ ����
                    if (v == 1) face.t2 = 0; // �⺻������ ����
                    if (v == 2) face.t3 = 0; // �⺻������ ����
                }

                // ���� �ε���
                if (idx.normal_index >= 0 && idx.normal_index < attrib.normals.size() / 3) {
                    if (v == 0) face.n1 = idx.normal_index;
                    if (v == 1) face.n2 = idx.normal_index;
                    if (v == 2) face.n3 = idx.normal_index;
                }
                else {
                    if (v == 0) face.n1 = 0; // �⺻������ ����
                    if (v == 1) face.n2 = 0; // �⺻������ ����
                    if (v == 2) face.n3 = 0; // �⺻������ ����
                }
            }

            index_offset += fv;
            model->faces.push_back(face);
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

            std::filesystem::path obj_path(filename);
            std::string obj_dir = obj_path.parent_path().string(); // OBJ ������ ��ġ�� ���丮
            // �ؽ�ó ��θ� MTL ������ ���丮 �������� �籸��
            mat.map_Kd = resolve_texture_path(obj_dir, material.diffuse_texname);
            // �ؽ�ó ���� Ȯ�� �� �ʱ�ȭ
            mat.hasTexture = !mat.map_Kd.empty();
            mat.textureID = 0; // �ؽ�ó ID�� ���� OpenGL���� �ε� �� ����

            model->materials.push_back(mat);
            load_texture(model->materials.back()); // �ؽ�ó �ε� �� OpenGL�� ���ε�
        }
    }
}