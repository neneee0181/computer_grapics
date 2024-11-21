#version 330 core
in vec3 FragPos;   // 프래그먼트 위치 (vertex shader에서 전달)
in vec3 Normal;    // 정점의 법선 벡터 (vertex shader에서 전달)
in vec2 TexCoords; // 텍스처 좌표 (vertex shader에서 전달)

out vec4 FragColor;

uniform vec3 viewPos;      // 카메라(뷰어) 위치
uniform vec3 lightPos;     // 광원의 위치
uniform vec3 lightColor;   // 광원의 색상

uniform vec3 Ka;           // 환경광 색상
uniform vec3 Ks;           // 반사광 색상
uniform float Ns;          // 반짝임 강도 (샤이니니스)

uniform sampler2D texture1; // 텍스처 샘플러
uniform bool hasTexture;    // 텍스처 유무 플래그

uniform bool isRigidBody;   // true면 rigid body 상태로 렌더링

void main() {

    if (isRigidBody) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 고정된 빨간색
        return;
    }

    // 1. 환경광 (Ambient)
    vec3 ambient = Ka * lightColor;

    // 2. 난반사광 (Diffuse)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // 텍스처가 있는 경우, 텍스처 색상을 사용하고, 그렇지 않으면 Kd 사용
    vec3 diffuseColor = hasTexture ? texture(texture1, TexCoords).rgb : vec3(1.0);
    vec3 diffuse = diffuseColor * diff * lightColor;

    // 3. 반사광 (Specular)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // 최종 색상 계산
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
