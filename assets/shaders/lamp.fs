#version 430 core

out vec4 FragColor;
uniform vec3 lightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;

void main(){
    FragColor = vec4(lightColor, 1.0);
}