#version 430 core

out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}