#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include "object.h"

namespace buffer {
    
    // works only for a triangulated mesh
    void load(const char* file, unsigned* nData, float* &data, unsigned* nIndices, unsigned int* &indices, object::TYPE type, float* materials);
}