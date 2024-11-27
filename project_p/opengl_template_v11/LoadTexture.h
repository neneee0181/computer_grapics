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

    // 텍스처 로드 시 Y축 뒤집기 설정
    stbi_set_flip_vertically_on_load(true);

    // 이미지 로드
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    // 에러 처리
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
        throw std::runtime_error("Failed to load texture");
    }

    // 텍스처 생성 및 설정
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return textureID;
}