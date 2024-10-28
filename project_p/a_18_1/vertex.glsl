#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Color;     // 정점 색상

out vec3 out_Color;    // 프래그먼트 쉐이더로 전달할 색상
uniform mat4 model;       // 모델 변환 행렬
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

uniform mat4 Line;
uniform mat4 Ss;
uniform int modelStatus;  // 0 -> 모델 , 1 -> 좌표계

void main() {
    if (modelStatus == 0) {
        gl_Position = projectionTransform * viewTransform * model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // 모델
    }else if(modelStatus == 1){
        gl_Position = projectionTransform * viewTransform * Line * vec4(in_Position, 1.0);
        out_Color = in_Color;   // x,y,z 축
    }else if(modelStatus == 2){
        gl_Position = projectionTransform * viewTransform * model * vec4(in_Position, 1.0);
        out_Color = vec3(0.5, 0.5, 0.5);
    }
}
