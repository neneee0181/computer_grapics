#version 330 core
in vec3 FragPos;   // 프래그먼트 위치 (vertex shader에서 전달)
in vec3 Normal;    // 정점의 법선 벡터 (vertex shader에서 전달)

out vec4 FragColor;

uniform vec3 viewPos;      // 카메라(뷰어) 위치
uniform vec3 lightPos;     // 광원의 위치
uniform vec3 lightColor;   // 광원의 색상

uniform vec3 Ka;           // 환경광 색상
uniform vec3 Kd;           // 난반사 색상
uniform vec3 Ks;           // 반사광 색상
uniform float Ns;          // 반짝임 강도 (샤이니니스)

uniform bool useCustomColor;  // 커스텀 색상 사용 플래그
uniform vec3 customColor;     // 커스텀 색상 (좌표축 색상)

void main() {
    if (useCustomColor) {
        // 플래그가 true일 경우, 조명 계산 없이 직접 색상 출력
        FragColor = vec4(customColor, 1.0);
        return;
    }

    // 조명 계산
    vec3 ambient = Ka * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Kd * diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // 최종 색상 계산
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
