#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Color;     // 정점 색상
layout (location = 2) in vec3 in_Normal;    // 법선 벡터
layout (location = 3) in vec2 in_TexCoord;  // 텍스처 좌표 추가

out vec3 out_Color;    // 프래그먼트 쉐이더로 전달할 색상
out vec3 FragPos;      // 프래그먼트 위치
out vec3 Normal;       // 프래그먼트 법선 벡터
out vec2 TexCoords;    // 프래그먼트 쉐이더로 전달할 텍스처 좌표 추가

uniform mat4 modelV;       // 모델 변환 행렬

uniform mat4 tre;
uniform mat4 xLine;
uniform mat4 yLine;
uniform mat4 zLine;
uniform int isLine;  // 선을 그릴 때는 1, 면을 그릴 때는 0

void main() {
    if (isLine == 0 || isLine == 1) {
        // 정육면체에 대한 변환 행렬을 적용
        gl_Position = modelV * vec4(in_Position, 1.0);

    } else if (isLine == 2) {
        // X축 선에 대한 변환 행렬 적용
        gl_Position = xLine * vec4(in_Position, 1.0);
    } else if (isLine == 3) {
        // Y축 선에 대한 변환 행렬 적용
        gl_Position = yLine * vec4(in_Position, 1.0);
    }  else if (isLine == 4) {
        // Y축 선에 대한 변환 행렬 적용
        gl_Position = zLine * vec4(in_Position, 1.0);
    } 

    // 선을 그릴 때는 흰색, 면을 그릴 때는 정점 색상 사용
    if (isLine == 1)
        out_Color = vec3(1.0, 1.0, 1.0);  // 선일 때는 흰색
    else if (isLine == 0)
        out_Color = in_Color;  // 면일 때는 입력된 색상
    else if (isLine == 2)
        out_Color = vec3(0.0, 1.0, 0.0);  // X축일 때는 녹색
    else if (isLine == 3)
        out_Color = vec3(1.0, 0.0, 0.0);  // Y축일 때는 빨간색
    else if (isLine == 4)
        out_Color = vec3(0.0, 0.0, 1.0);  // z축일 때는 파란색
}
