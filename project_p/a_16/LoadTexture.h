#pragma once
#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)

#define STB_IMAGE_IMPLEMENTATION
#include <gl/stb_image.h>

// 텍스처 로드 함수
GLuint loadTexture(const char* texturePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);  // 텍스처 ID 생성
    glBindTexture(GL_TEXTURE_2D, textureID);  // 2D 텍스처 바인딩

    // 텍스처 반복 방식 및 필터링 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);  // 이미지 로드
    if (data) {
        // 이미지 데이터를 텍스처로 업로드 (RGBA 또는 RGB 포맷 처리)
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  // Mipmap 생성
    }
    else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }
    stbi_image_free(data);  // 이미지 메모리 해제

    return textureID;
}