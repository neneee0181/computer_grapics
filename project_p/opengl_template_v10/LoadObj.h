#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "LoadMtl.h"
#include "tiny_obj_loader.h"

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
        nullptr          // MTL 파일의 기본 경로(NULL이면 OBJ 파일과 같은 경로에서 MTL 로드)
    );

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

    // **도형 데이터 (faces) 추가**
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f]; // Face의 정점 개수 (3 또는 4)

            // Face 데이터 초기화
            Face face;
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;

            // Face의 Material Index 설정
            if (shape.mesh.material_ids[f] >= 0) {
                face.materialIndex = shape.mesh.material_ids[f];
            }
            else {
                face.materialIndex = -1; // Material이 없는 경우 기본값 -1
            }

            // Face의 각 정점 처리
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // 정점 인덱스 추가
                vertexIndices.push_back(idx.vertex_index);

                // 텍스처 좌표 인덱스 추가
                if (idx.texcoord_index >= 0) {
                    texCoordIndices.push_back(idx.texcoord_index);
                }
                else {
                    texCoordIndices.push_back(0); // 텍스처 좌표가 없는 경우 기본값
                }

                // 법선 인덱스 추가
                if (idx.normal_index >= 0) {
                    normalIndices.push_back(idx.normal_index);
                }
                else {
                    normalIndices.push_back(0); // 법선이 없는 경우 기본값
                }
            }

            // 삼각형 Face 처리
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
            // 사각형 Face 처리 (두 개의 삼각형으로 분할)
            else if (fv == 4) {
                // 첫 번째 삼각형
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

                // 두 번째 삼각형
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

            index_offset += fv; // Face 정점 수만큼 인덱스 오프셋 증가
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

            // 텍스처 유무 확인 및 초기화
            mat.hasTexture = !mat.map_Kd.empty();
            mat.textureID = 0; // 텍스처 ID는 이후 OpenGL에서 로드 후 설정

            model->materials.push_back(mat);
        }
    }
}