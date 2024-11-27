#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include <gl/stb_image.h>

// 텍스처 로드 함수
GLuint load_texture(const std::string& path) {
    int width, height, nrChannels;

    // 이미지 로드
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    // 텍스처 로드 실패 처리
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl; // 추가 디버깅 메시지
        throw std::runtime_error("Texture loading failed: " + path); // 예외 발생
    }

    // OpenGL에 텍스처 생성
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 텍스처 파라미터 설정 (필터링, 반복 모드)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 텍스처 데이터 업로드
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA; // 채널 수에 따라 포맷 선택
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Mipmap 생성

    // 메모리 해제
    stbi_image_free(data);

    return textureID; // 텍스처 ID 반환
}