#pragma once

#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <vector>

bool Import3DFromFile(const std::string& pFile);
std::vector<struct MyMesh> createMeshFromAssimp(const aiScene* sc);
