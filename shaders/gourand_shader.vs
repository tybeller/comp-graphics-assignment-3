#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 FragPos = vec3((model * vec4(aPos, 1.0)).xyz);
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 lightColor = vec3(1.0f);
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 res = (ambient + diffuse + specular) * aColor;
    ourColor = res;
}
