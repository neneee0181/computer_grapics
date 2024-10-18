#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Color;     // 정점 색상
layout (location = 2) in vec3 in_Normal;    // 법선 벡터
layout (location = 3) in vec2 in_TexCoord;  // 텍스처 좌표 추가

out vec3 out_Color;    // 프래그먼트 쉐이더로 전달할 색상
out vec3 FragPos;      // 프래그먼트 위치
out vec3 Normal;       // 프래그먼트 법선 벡터
out vec2 TexCoords;    // 프래그먼트 쉐이더로 전달할 텍스처 좌표 추가

uniform mat4 model;       // 모델 변환 행렬

uniform mat4 tre;
uniform mat4 xLine;
uniform mat4 yLine;
uniform mat4 zLine;

uniform mat4 Line;
uniform int modelStatus;  // 선을 그릴 때는 1, 면을 그릴 때는 0

void main() {
    if (modelStatus == 0) {
        gl_Position = model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // 면일 때는 입력된 색상
    }else if(modelStatus == 1){
        gl_Position = Line * vec4(in_Position, 1.0);
        out_Color = in_Color;  // 면일 때는 입력된 색상
    }
}
