#version 330 core

layout(location = 0) in vec3 in_Position;  // 정점 위치
layout(location = 1) in vec3 in_Color;     // 정점 색상

out vec3 out_Color;  // 프래그먼트 쉐이더로 전달할 색상
uniform mat4 model;  // 모델 변환 행렬
uniform mat4 Line;   // 좌표 축용 변환 행렬
uniform int modelStatus;  // 0 -> 모델, 1 -> 좌표계

// X축 회전 행렬 생성 함수
mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c,   -s,  0.0,
        0.0, s,   c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

// Y축 회전 행렬 생성 함수
mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,   0.0, s,   0.0,
        0.0, 1.0, 0.0, 0.0,
        -s,  0.0, c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    // 추가 회전 변환 행렬 적용 (모델 좌표계에서 회전)
    mat4 rotationMatrix = mat4(1.0);  // 단위 행렬로 시작
    
    // 35도 X축 회전
    float angleX = radians(35.0);
    rotationMatrix = rotateX(angleX);  // X축 회전

    // -35도 Y축 회전
    float angleY = radians(-35.0);
    rotationMatrix = rotationMatrix * rotateY(angleY);  // Y축 회전

    // 모델에 회전 변환을 적용

    if (modelStatus == 0) {
        // 모델 변환 적용
        gl_Position = model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // 모델 색상 전달
    } else if (modelStatus == 1) {
        // 좌표계 변환 적용
        mat4 newLine = Line * rotationMatrix;
        gl_Position = newLine * vec4(in_Position, 1.0);
        out_Color = in_Color;  // 좌표계 색상 전달
    }
}
