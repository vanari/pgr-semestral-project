#pragma once
#include "object.h"
#include "shader.h"

class Baseline {
    protected:
    int edgeSize = 1;
    int xCenter = 0;
    int yCenter = 0;
    float zLvl = -2.0f;
    float tileSize = 10.0f;
    float *vertexBuffer = nullptr;
    object *baselineObj = nullptr;

    public:
    Baseline() = delete;
    Baseline(shaderProgram &shader);
    ~Baseline();

    void setCenter(float x, float y, bool refill = true);
    void draw();
    void fillBuffer();

    protected:
    void generateTile(int x, int y, int& index);


};