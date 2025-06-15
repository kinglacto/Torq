#include "lamp.h"

void Lamp::render(){
    shader->set3Float("lightColor", lightColor);
    Sphere::render();
}