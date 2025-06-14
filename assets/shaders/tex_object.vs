#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
out vec2 TexCoord;

void main(){
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    Color = aColor;
    TexCoord = aTexCoord;
}