#pragma once

// To Do: create an asset pipeline that stores paths to shaders/textures in a json file and dynamically
// generates enum classes with the appropriate names at runtime. 

#include <shader.h>
#include <texture.h>
#include <bits/stdc++.h>

class ResourceManager{
public:
    using ResourceID = int;

    ResourceManager() = default;
    static Shader* LoadShader(const std::string& vPath, const std::string& fPath, ResourceID id);
    static Shader* GetShader(ResourceID id);
    static void DeleteShader(ResourceID id);

    static Texture* LoadTexture(const std::string& path, ResourceID id);
    static Texture* GetTexture(ResourceID id);
    static void DeleteTexture(ResourceID id);

    static void deleteAllShaders();
    static void deleteAllTextures();

    static void deleteAll();

private:
    static ResourceID HashName(ResourceID id);

    static std::unordered_map<ResourceID, Shader> shaders;
    static std::unordered_map<ResourceID, Texture> textures;
};