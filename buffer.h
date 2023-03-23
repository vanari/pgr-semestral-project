#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include "object.h"

namespace buffer {
    
    // works only for a triangulated mesh
    unsigned int load(const char* file, float* &data, unsigned int* &&indices, object::TYPE type);
}