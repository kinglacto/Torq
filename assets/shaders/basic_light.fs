#version 430 core

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main() {
    vec4 fcolor = texture(texture1, TexCoord);
    vec3 texColor = fcolor.rgb;

    vec3 ambient = light.ambient * texColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * texColor;

    //FragColor = vec4(vec3(ambient + diffuse), 1.0);
    FragColor = fcolor;
}