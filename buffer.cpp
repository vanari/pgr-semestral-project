#include "buffer.h"

// works only for a triangulated mesh
// NOT ANYMORE?
void buffer::load(const char* file, unsigned *nData, float* &data, unsigned *nIndices, unsigned int* &indices, object::TYPE type) {
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

    unsigned int meshSize = (mesh->mNumVertices)*(type);

    std::cout << "Number of vertices in mesh: " << (mesh->mNumVertices) << std::endl;

    data = new float[meshSize];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        unsigned k = 0;

        std::cout << "bundle " << i+1 << ':' << std::endl;

        aiVector3t vertex = mesh->mVertices[i];
        data[type*i+k++] = vertex.x;
        data[type*i+k++] = vertex.y;
        data[type*i+k++] = vertex.z;

        std::cout << vertex.x << ' ' << vertex.y << ' ' << vertex.z << std::endl;

        if (type != object::NONE) {
            aiVector3t normal = mesh->mNormals[i];
            data[type*i+k++] = normal.x;
            data[type*i+k++] = normal.y;
            data[type*i+k++] = normal.z;
            std::cout << normal.x << ' ' << normal.y << ' ' << normal.z << std::endl;
        }

        if (type == object::COLORED || type == object::TEXTURED_COLORED) {
            aiColor4t color = mesh->mColors[0][i];
            data[type*i+k++] = color.r;
            data[type*i+k++] = color.g;
            data[type*i+k++] = color.b;
        }
        
        if (type == object::TEXTURED || type == object::TEXTURED_COLORED) {
            aiVector3t texCoords = mesh->mTextureCoords[0][i];
            data[type*i+k++] = texCoords.x;
            data[type*i+k++] = texCoords.y;
            std::cout << texCoords.x << ' ' << texCoords.y << ' ' << texCoords.z << std::endl;
        }
        
    }

    if (nIndices != nullptr) {
        indices = new unsigned[3 * (mesh->mNumFaces)];

        for (unsigned int i = 0; i < (mesh->mNumFaces); ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices[3*i + j] = face.mIndices[j];
                std::cout << "index: " << face.mIndices[j]+1 << ' ' << i << std::endl;
            }
        }
        *nIndices = 3 * (mesh->mNumFaces);
    }

    *nData = mesh->mNumVertices;

}
