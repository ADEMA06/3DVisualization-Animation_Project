#pragma once

#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <vector>

const aiScene* Import3DFromFile(const std::string& pFile, const aiScene* sc, vec3* max_vertex, vec3* min_vertex);
std::vector<struct MyMesh> createMeshFromAssimp(const aiScene* sc);
