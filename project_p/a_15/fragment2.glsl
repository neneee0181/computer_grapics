#version 330 core

struct Material {
    vec3 Ka;  // 환경광 색상
    vec3 Kd;  // 난반사광 색상
    vec3 Ks;  // 반사광 색상
    float Ns; // 반짝임 강도
};

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

in vec2 TexCoords;  // 텍스처 좌표 입력
uniform sampler2D texture1;  // 텍스처 유니폼

void main() {
    // 텍스처 색상 샘플링
    vec3 texColor = texture(texture1, TexCoords).rgb;

    // 조명 계산 (Phong 조명 모델)
    vec3 ambient = material.Ka * texColor;  // 텍스처와 환경광 조합

    // Diffuse (난반사광 계산)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.Kd * diff * texColor;  // 텍스처와 난반사광 조합

    // Specular (반사광 계산)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    vec3 specular = material.Ks * spec;  // 반사광은 텍스처와 무관하게 적용

    // 최종 색상 계산
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);  // 알파값은 1.0으로 설정
}
