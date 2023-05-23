#pragma once
#include <stdint.h>
#include "shader.h"
#include "object.h"
#include <noise/noise.h>
#include <random>
#include <GLFW/glfw3.h>
#include "buffer.h"

class Terrain {
    protected:
    int nOfVertices = 0;
    const static int edgeSize = 13;
    int xCenter = 0;
    int yCenter = 0;
    float zLvl = -2.0f;
    float zStep = 1.0f;
    float tileSize = 10.0f;
    float* vertexBuffer = nullptr;
    object* terrainObject = nullptr;
    object* shrooms[50*50];

    noise::module::Perlin textureGenerator; 

    noise::module::Perlin terrainGenerator;
    int seed = 256;
    double frequency = 0.05;
    double amplitude = 10;

    int nTextureTiles = 2;

    int mapSize = 3;
    const uint8_t testMap[9] = { 0x11, 0x01, 0x02,
                                0x11, 0x22, 0x01,
                                0x02, 0x00, 0x00 };

    void fillBuffer();
    void generateTile(int x, int y, int& index);
    void generateXInterTile(int x, int y, int& index);
    void generateYInterTile(int x, int y, int& index);
    void generateDiagInterTile(int x, int y, int& index);
    float getTileLvl(int x, int y);
    float getTextureX(int x, int y);
    float getTextureY(int x, int y);
    object* generateShroom(int x, int y);

    public:
    shaderProgram* pShroomShader;
    float getZlvl(float x, float y) { return zLvl + getTileLvl(static_cast<int>(x/tileSize - .5f), static_cast<int>(y/tileSize - .5f)); };
    Terrain(shaderProgram &shader, const char* textureFile); 
    void setCenter(float x, float y, bool refill = true);
    void draw();

};