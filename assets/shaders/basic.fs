#version 430 core

out vec4 FragColor;
in vec3 uColor;

void main() {
    FragColor = vec4(uColor.xyz, 1.0);
}