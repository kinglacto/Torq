// To Do: create an asset pipeline that stores paths to shaders/textures in a json file and dynamically
// generates enum classes with the appropriate names at runtime. 

#pragma once

#define BASIC_VERTEX_SHADER "assets/shaders/basic.vs"
#define BASIC_TEXTURE_VERTEX_SHADER "assets/shaders/basic_texture.vs"
#define BASIC_LIGHT_VERTEX_SHADER "assets/shaders/basic_light.vs"

#define BASIC_FRAG_SHADER "assets/shaders/basic.fs"
#define BASIC_TEXTURE_FRAG_SHADER "assets/shaders/basic_texture.fs"
#define BASIC_LIGHT_FRAG_SHADER "assets/shaders/basic_light.fs"

#define ASSETS_DIR "assets"

#define LAMP_FRAG_SHADER "assets/shaders/lamp.fs"

#define ATLAS_DIR "assets/atlas"
#define TEXTURE_DIR "assets/textures"
#define CHUNK_DIR "assets/chunks"