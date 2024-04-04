#version 330 core
out vec4 FragColor;
in vec3 ourColor;

uniform vec3 lightColor;

void main() {
    FragColor = vec4(vec3(gl_FragCoord.z) * vec3(0.9f), 1.0f);
}
