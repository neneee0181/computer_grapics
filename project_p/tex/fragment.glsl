// fragment.glsl
#version 330 core
out vec4 FragColor;

uniform vec3 Kd;  // 난반사 색상
uniform vec3 Ka;  // 환경광 색상
uniform vec3 Ks;  // 반사광 색상

void main() {
    vec3 color = Kd;  // 간단히 난반사 색상 사용 (Phong 모델 확장 가능)
    FragColor = vec4(color, 1.0);
}