#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Color;     // 정점 색상

out vec3 out_Color;  // 프래그먼트 쉐이더로 전달할 색상
uniform mat4 cube;   // 변환 행렬
uniform int isLine;  // 선을 그릴 때는 1, 면을 그릴 때는 0

void main() {
    gl_Position = cube * vec4(in_Position, 1.0);
    
    // 선을 그릴 때는 검정색, 면을 그릴 때는 정점 색상 사용
    if (isLine == 1)
        out_Color = vec3(0.0, 0.0, 0.0);  // 선일 때는 검정색
    else
        out_Color = in_Color;  // 면일 때는 입력된 색상
}
