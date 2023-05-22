#include "baseline.h"
#include "shader.h"

Baseline::Baseline(shaderProgram &shader) {
    vertexBuffer = new float[edgeSize * edgeSize * 6 * object::TEXTURED];    
    fillBuffer();
    baselineObj = new object(edgeSize * edgeSize * 6 * object::TEXTURED, vertexBuffer, edgeSize*edgeSize*6, shader);
    baselineObj->loadTexture("assets/tile.jpg", object::RGB, 0);
}

Baseline::~Baseline() {
    //delete vertexBuffer;
    //delete baselineObj;
}

void Baseline::fillBuffer() {
    int q = 0;
    for (int i = xCenter - (edgeSize/2); i < xCenter + (edgeSize+1)/2; ++i) {
        for (int j = yCenter - (edgeSize/2); j < yCenter + (edgeSize+1)/2; ++j) {
            generateTile(i, j, q);
        }
    }

    if (baselineObj != nullptr)
        baselineObj->refillBuffers(edgeSize * edgeSize * 6 * object::TEXTURED, vertexBuffer);
}

void Baseline::generateTile(int x, int y, int& index) {

    const float xOffset[] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    const float yOffset[] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    for (int vertex = 0; vertex < 6; vertex++) {
            //vertex
        vertexBuffer[index++] = static_cast<float>(x) * tileSize + xOffset[vertex] * tileSize;
        vertexBuffer[index++] = static_cast<float>(y) * tileSize + yOffset[vertex] * tileSize;
        vertexBuffer[index++] = zLvl;

        //normal
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 0.0f;
        vertexBuffer[index++] = 1.0f;

        //texture
        vertexBuffer[index++] = xOffset[vertex];
        vertexBuffer[index++] = yOffset[vertex];
    }
}

void Baseline::setCenter(float x, float y, bool refill) {
    int pXCenter = xCenter;
    int pYCenter = yCenter;
    x = x < 0.0f ? x - tileSize : x;
    y = y < 0.0f ? y - tileSize : y;
    xCenter = static_cast<int>(x/(tileSize));
    yCenter = static_cast<int>(y/(tileSize));
    std::cout << "center: " << xCenter << " " << yCenter <<std::endl;

    if (refill) {
        if (pXCenter != xCenter || pYCenter != yCenter) {
            fillBuffer();
        }
    }
}

void Baseline::draw() {
    baselineObj->draw();
}