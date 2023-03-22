#include "buffer.h"

void buffer::loadNoEBO(const char* file, float* &data) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(file,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_ValidateDataStructure |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_SortByPType |
        aiProcess_FindInvalidData |
        aiProcess_GenUVCoords |
        aiProcess_TransformUVCoords |
        aiProcess_FindInstances
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error: failed to load file: " << file << std::endl;
	}

    aiMesh* mesh = scene->mMeshes[0];

    unsigned int meshSize = (mesh->mNumVertices)*(3*3+2);

    std::cout << "Number of vertices in mesh: " << (mesh->mNumVertices) << std::endl;

    std::cout << "debug 1" << std::endl;

    data = new float[meshSize];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3t vertex = mesh->mVertices[i];
        data[11*i] = vertex.x;
        data[11*i+1] = vertex.y;
        data[11*i+2] = vertex.z;


        std::cout << "vertex done" << std::endl;

        aiVector3t normal = mesh->mNormals[i];
        data[11*i+3] = normal.x;
        data[11*i+4] = normal.y;
        data[11*i+5] = normal.z;
        
        std::cout << "normal done" << std::endl;


        //aiColor4t color = mesh->mColors[0][i];
        data[11*i+6] = 1.0f;//color.r;
        data[11*i+7] = 0.3f;//color.g;
        data[11*i+8] = 0.0f;//color.b;
        
        std::cout << "color done" << std::endl;


        aiVector3t texCoords = mesh->mTextureCoords[0][i];
        data[11*i+9] = texCoords.x;
        data[11*i+10] = texCoords.y;
        
        std::cout << "texture done" << std::endl;


    }
    
    std::cout << "debug 2" << std::endl;

}