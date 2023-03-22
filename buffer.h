#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

namespace buffer {
    void loadNoEBO(const char* file, float* &data);
}