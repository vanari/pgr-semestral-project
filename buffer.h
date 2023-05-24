/*
   File: buffer.h
   Description: Handles loading objects from files.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include "object.h"

namespace buffer {

    // allocatess space for buffers from file
    // -- file - filename
    // -- nData - number of vertices
    // -- data - VBA to be filled
    // -- nIndices - number of indices
    // -- indices - EBO to be filled
    // -- type - type of object
    // -- materials - buffer with material properties 
    void load(const char* file, unsigned* nData, float* &data, unsigned* nIndices, unsigned int* &indices, object::TYPE type, float* materials);
}