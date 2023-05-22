#include "terrain.h"

Terrain::Terrain(shaderProgram& shader, const char* textureFile) {

    textureGenerator.SetSeed(seed);
    textureGenerator.SetFrequency(frequency);

    terrainGenerator.SetSeed(seed);
    terrainGenerator.SetFrequency(frequency);

    nOfVertices = edgeSize * edgeSize * 6 + (edgeSize-1) * (edgeSize) * 6 * 2 + (edgeSize-1) * (edgeSize-1) * 12;
    vertexBuffer = new float[nOfVertices * object::TEXTURED];    
    fillBuffer();
    terrainObject = new object(nOfVertices * object::TEXTURED, vertexBuffer, nOfVertices, shader);
    terrainObject->loadTexture(textureFile, object::RGB, 0);
}

void Terrain::fillBuffer() {
    int q = 0;
    for (int i = xCenter - (edgeSize/2); i < xCenter + (edgeSize+1)/2; ++i) {
        for (int j = yCenter - (edgeSize/2); j < yCenter + (edgeSize+1)/2; ++j) {
            generateTile(i, j, q);
        }
    }

    for (int i = xCenter - (edgeSize/2); i < xCenter + (edgeSize+1)/2 - 1; ++i) {
        for (int j = yCenter - (edgeSize/2); j < yCenter + (edgeSize+1)/2; ++j) {
            generateXInterTile(i, j, q);
        }
    }
    
    for (int i = xCenter - (edgeSize/2); i < xCenter + (edgeSize+1)/2; ++i) {
        for (int j = yCenter - (edgeSize/2); j < yCenter + (edgeSize+1)/2 - 1; ++j) {
            generateYInterTile(i, j, q);
        }
    }
    
    for (int i = xCenter - (edgeSize/2); i < xCenter + (edgeSize+1)/2 - 1; ++i) {
        for (int j = yCenter - (edgeSize/2); j < yCenter + (edgeSize+1)/2 - 1; ++j) {
            generateDiagInterTile(i, j, q);
        }
    }

    if (terrainObject != nullptr)
        terrainObject->refillBuffers(nOfVertices * object::TEXTURED, vertexBuffer);
}

void Terrain::generateDiagInterTile(int x, int y, int& index) {

    const float xOffset[] = {.5f, 0.0f, 0.0f, .5f, 0.0f, 1.0f, .5f, 1.0f, 1.0f, .5f, 1.0f, 0.0f};
    const float yOffset[] = {.5f, 0.0f, 1.0f, .5f, 1.0f, 1.0f, .5f, 1.0f, 0.0f, .5f, 0.0f, 0.0f};

    //if (getTileLvl(x, y) == getTileLvl(x, y+1))
    //    return;

    float tileLvl = zLvl + getTileLvl(x, y);
    float maxLvl = std::max(std::max(getTileLvl(x,y), getTileLvl(x+1,y)), std::max(getTileLvl(x,y+1), getTileLvl(x+1,y+1)));
    float minLvl = std::min(std::min(getTileLvl(x,y), getTileLvl(x+1,y)), std::min(getTileLvl(x,y+1), getTileLvl(x+1,y+1)));
    float midLvl = zLvl + (maxLvl + minLvl) / 2;
    //float midLvl = zLvl + ( getTileLvl(x,y) + getTileLvl(x+1,y) + getTileLvl(x,y+1) + getTileLvl(x+1,y+1) )/4.0f;

    for (int vertex = 0; vertex < 12; vertex++) {
            //vertex
        vertexBuffer[index++] = (static_cast<float>(x)) * tileSize + 0.9f * tileSize + xOffset[vertex] * 0.2f * tileSize;
        vertexBuffer[index++] = (static_cast<float>(y)) * tileSize + 0.9f * tileSize + yOffset[vertex] * 0.2f * tileSize;
        vertexBuffer[index++] = (vertex % 3 == 0) ? midLvl : zLvl + getTileLvl(x + static_cast<int>(xOffset[vertex]), y + static_cast<int>(yOffset[vertex]));

        //normal
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 1.0f;

        //texture
        vertexBuffer[index++] = getTextureX(x, y) + xOffset[vertex]/static_cast<float>(nTextureTiles);
        vertexBuffer[index++] = getTextureY(x, y) + yOffset[vertex]/static_cast<float>(nTextureTiles);
    }
}

void Terrain::generateYInterTile(int x, int y, int& index) {

    const float xOffset[] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    const float yOffset[] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    //if (getTileLvl(x, y) == getTileLvl(x, y+1))
    //    return;

    float tileLvl = zLvl + getTileLvl(x, y);

    for (int vertex = 0; vertex < 6; vertex++) {
            //vertex
        vertexBuffer[index++] = (static_cast<float>(x)) * tileSize + 0.1f * tileSize + xOffset[vertex] * 0.8f * tileSize;
        vertexBuffer[index++] = (static_cast<float>(y)) * tileSize + 0.9f * tileSize + yOffset[vertex] * 0.2f * tileSize;
        vertexBuffer[index++] = zLvl + getTileLvl(x, y + static_cast<int>(yOffset[vertex]));

        //normal
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 1.0f;

        //texture
        vertexBuffer[index++] = getTextureX(x, y) + xOffset[vertex]/static_cast<float>(nTextureTiles);
        vertexBuffer[index++] = getTextureY(x, y) + yOffset[vertex]/static_cast<float>(nTextureTiles);
    }
}

void Terrain::generateXInterTile(int x, int y, int& index) {

    const float xOffset[] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    const float yOffset[] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    //if (getTileLvl(x+1, y) == getTileLvl(x, y))
    //    return;

    float tileLvl = zLvl + getTileLvl(x, y);

    for (int vertex = 0; vertex < 6; vertex++) {
            //vertex
        vertexBuffer[index++] = (static_cast<float>(x)) * tileSize + 0.9f * tileSize + xOffset[vertex] * 0.2f * tileSize;
        vertexBuffer[index++] = (static_cast<float>(y)) * tileSize + 0.1f * tileSize + yOffset[vertex] * 0.8f * tileSize;
        vertexBuffer[index++] = zLvl + getTileLvl(x + static_cast<int>(xOffset[vertex]), y);

        //normal
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 1.0f;

        //texture
        vertexBuffer[index++] = getTextureX(x, y) + xOffset[vertex]/static_cast<float>(nTextureTiles);
        vertexBuffer[index++] = getTextureY(x, y) + yOffset[vertex]/static_cast<float>(nTextureTiles);
    }
}

void Terrain::generateTile(int x, int y, int& index) {

    const float xOffset[] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    const float yOffset[] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    float tileLvl = zLvl + getTileLvl(x, y);

    for (int vertex = 0; vertex < 6; vertex++) {
            //vertex
        vertexBuffer[index++] = static_cast<float>(x) * tileSize + 0.1f * tileSize + xOffset[vertex] * 0.8f * tileSize;
        vertexBuffer[index++] = static_cast<float>(y) * tileSize + 0.1f * tileSize + yOffset[vertex] * 0.8f * tileSize;
        vertexBuffer[index++] = tileLvl;

        //normal
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 1.0f;

        //texture
        vertexBuffer[index++] = getTextureX(x, y) + xOffset[vertex]/static_cast<float>(nTextureTiles);
        vertexBuffer[index++] = getTextureY(x, y) + yOffset[vertex]/static_cast<float>(nTextureTiles);
    }
}

float Terrain::getTileLvl(int x, int y) {
    if (false) {
        if (x < -(mapSize/2) || x > mapSize/2) {
            return 0;
        }
        
        if (y < -(mapSize/2) || y > mapSize/2) {
            return 0;
        }        

        return static_cast<float>(testMap[(x+mapSize/2) + (y+mapSize/2) * mapSize] >> 4)*zStep;
    } else {
        double value = terrainGenerator.GetValue(static_cast<double>(x), static_cast<double>(y), 0);
        return static_cast<int>(value * amplitude);
    }
}

void Terrain::setCenter(float x, float y, bool refill) {
    int pXCenter = xCenter;
    int pYCenter = yCenter;
    x = x < 0.0f ? x - tileSize : x;
    y = y < 0.0f ? y - tileSize : y;
    xCenter = static_cast<int>(x/(tileSize));
    yCenter = static_cast<int>(y/(tileSize));
    //std::cout << "center: " << xCenter << " " << yCenter <<std::endl;

    if (refill) {
        if (pXCenter != xCenter || pYCenter != yCenter) {
            fillBuffer();
        }
    }
}

void Terrain::draw() {
    terrainObject->draw();
}

float Terrain::getTextureX(int x, int y) {
    
    uint8_t texVal = 0;

    if (false) {
        if (x < -(mapSize/2) || x > mapSize/2) {
            return 0;
        }
        
        if (y < -(mapSize/2) || y > mapSize/2) {
            return 0;
        }

        texVal = testMap[(x+mapSize/2) + (y+mapSize/2) * mapSize] << 4;
        texVal = texVal >> 4;
    } else {
        //texVal = 
        texVal = static_cast<uint8_t>(2*(1.0 + textureGenerator.GetValue(static_cast<double>(x),static_cast<double>(y),0)));
    }

    return static_cast<float>(texVal % nTextureTiles)/static_cast<float>(nTextureTiles);
}

float Terrain::getTextureY(int x, int y) {
    
    uint8_t texVal = 0;

    if (false) {
        if (x < -(mapSize/2) || x > mapSize/2) {
            return 0;
        }
        
        if (y < -(mapSize/2) || y > mapSize/2) {
            return 0;
        }

        texVal = testMap[(x+mapSize/2) + (y+mapSize/2) * mapSize] << 4;
        texVal = texVal >> 4;
    } else {
        texVal = static_cast<uint8_t>(2*(1.0 + textureGenerator.GetValue(static_cast<double>(x),static_cast<double>(y),0)));
        //texVal = static_cast<uint8_t>(3*textureGenerator.GetValue(static_cast<double>(x),static_cast<double>(y),0));
        //std::cout << "texval " << int(texVal) << " " << x << std::endl;
    }

    return static_cast<float>(texVal / nTextureTiles)/static_cast<float>(nTextureTiles);
}