#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem> // C++17에서 파일 경로 처리를 위한 헤더

#include "Model.h"
#include "LoadMtl.h"
#include "LoadTexture.h"
#include "tiny_obj_loader.h"

// 텍스처 경로를 MTL 파일의 디렉토리 기준으로 처리하는 함수
std::string resolve_texture_path(const std::string& base_path, const std::string& texture_path) {
    // 텍스처 경로가 절대 경로인지 확인
    if (std::filesystem::path(texture_path).is_absolute()) {
        return texture_path;
    }

    // 텍스처 경로가 상대 경로라면 MTL 파일 경로 기준으로 결합
    std::filesystem::path base(base_path);
    std::filesystem::path texture(texture_path);
    return (base / texture).string();
}

// tinyobjloader를 이용하여 OBJ 파일을 읽는 함수
void read_obj_file(const std::string& filename, Model* model, const std::string& name, const std::string& type) {
    tinyobj::attrib_t attrib; // 정점, 텍스처 좌표, 법선 데이터를 저장하는 구조체
    std::vector<tinyobj::shape_t> shapes; // 도형(Shape) 리스트
    std::vector<tinyobj::material_t> materials; // 재질(Material) 리스트
    std::string warn, err; // 경고와 에러 메시지 저장

    // tinyobjloader로 OBJ 파일 로드
    bool ret = tinyobj::LoadObj(
        &attrib,       // 정점, 텍스처 좌표, 법선 데이터가 저장됨
        &shapes,       // 도형(face) 데이터가 저장됨
        &materials,    // 재질(material) 데이터가 저장됨
        &warn,         // 경고 메시지
        &err,          // 에러 메시지
        filename.c_str(), // OBJ 파일 경로
        "./obj/"          // MTL 파일의 기본 경로(NULL이면 OBJ 파일과 같은 경로에서 MTL 로드)
    );

    // 파일 로드 결과 확인
    if (!ret) {
        std::cerr << "ERROR: Failed to load OBJ file: " << err << std::endl;
        return;
    }

    // 경고 메시지 출력
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    // 에러 메시지 출력 및 실패 처리
    if (!err.empty()) {
        std::cerr << "ERROR: " << err << std::endl;
        if (!ret) {
            return;
        }
    }

    // 데이터 크기 확인
    std::cout << "Number of vertices: " << attrib.vertices.size() / 3 << std::endl;
    std::cout << "Number of texcoords: " << attrib.texcoords.size() / 2 << std::endl;
    std::cout << "Number of normals: " << attrib.normals.size() / 3 << std::endl;
    std::cout << "Number of shapes: " << shapes.size() << std::endl;
    std::cout << "Number of materials: " << materials.size() << std::endl;

    // 모델 이름 및 타입 설정
    model->name = name;
    model->type = type;

    // **정점 데이터 추가**
    for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
        Vertex vertex;
        vertex.x = attrib.vertices[3 * i + 0];
        vertex.y = attrib.vertices[3 * i + 1];
        vertex.z = attrib.vertices[3 * i + 2];
        model->vertices.push_back(vertex);
    }

    // **텍스처 좌표 데이터 추가**
    for (size_t i = 0; i < attrib.texcoords.size() / 2; i++) {
        TextureCoord texCoord;
        texCoord.u = attrib.texcoords[2 * i + 0];
        texCoord.v = attrib.texcoords[2 * i + 1];
        model->texCoords.push_back(texCoord);
    }

    // **법선 데이터 추가**
    for (size_t i = 0; i < attrib.normals.size() / 3; i++) {
        Normal normal;
        normal.nx = attrib.normals[3 * i + 0];
        normal.ny = attrib.normals[3 * i + 1];
        normal.nz = attrib.normals[3 * i + 2];
        model->normals.push_back(normal);
    }

    // **Face 데이터 추가**
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];
            Face face;

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // 정점 인덱스
                if (v == 0) face.v1 = idx.vertex_index;
                if (v == 1) face.v2 = idx.vertex_index;
                if (v == 2) face.v3 = idx.vertex_index;

                // 텍스처 좌표 인덱스
                if (idx.texcoord_index >= 0 && idx.texcoord_index < attrib.texcoords.size() / 2) {
                    if (v == 0) face.t1 = idx.texcoord_index;
                    if (v == 1) face.t2 = idx.texcoord_index;
                    if (v == 2) face.t3 = idx.texcoord_index;
                }
                else {
                    if (v == 0) face.t1 = 0; // 기본값으로 설정
                    if (v == 1) face.t2 = 0; // 기본값으로 설정
                    if (v == 2) face.t3 = 0; // 기본값으로 설정
                }

                // 법선 인덱스
                if (idx.normal_index >= 0 && idx.normal_index < attrib.normals.size() / 3) {
                    if (v == 0) face.n1 = idx.normal_index;
                    if (v == 1) face.n2 = idx.normal_index;
                    if (v == 2) face.n3 = idx.normal_index;
                }
                else {
                    if (v == 0) face.n1 = 0; // 기본값으로 설정
                    if (v == 1) face.n2 = 0; // 기본값으로 설정
                    if (v == 2) face.n3 = 0; // 기본값으로 설정
                }
            }

            index_offset += fv;
            model->faces.push_back(face);
        }
    }

    // **재질(Material) 처리**
    if (!materials.empty()) {
        for (const auto& material : materials) {
            Material mat;

            mat.Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
            mat.Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            mat.Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
            mat.Ns = material.shininess;
            mat.map_Kd = material.diffuse_texname;

            std::filesystem::path obj_path(filename);
            std::string obj_dir = obj_path.parent_path().string(); // OBJ 파일이 위치한 디렉토리
            // 텍스처 경로를 MTL 파일의 디렉토리 기준으로 재구성
            mat.map_Kd = resolve_texture_path(obj_dir, material.diffuse_texname);
            // 텍스처 유무 확인 및 초기화
            mat.hasTexture = !mat.map_Kd.empty();
            mat.textureID = 0; // 텍스처 ID는 이후 OpenGL에서 로드 후 설정

            model->materials.push_back(mat);
            load_texture(model->materials.back()); // 텍스처 로드 및 OpenGL에 업로드
        }
    }
}