#ifndef LAMP_H
#define LAMP_H

#include "sphere.h"

class Lamp: public Sphere{
public:
    glm::vec3 lightColor;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Lamp(glm::vec3 lightColor, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 pos, 
        float size):
    lightColor(lightColor), ambient(ambient), diffuse(diffuse), specular(specular),
    Sphere(pos, size)
    {

    } 

    void render();
};

#endif 