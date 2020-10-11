#pragma once

#include <vector>
#include <iostream>

#include "./glm/glm.hpp"
#include "./wavefunction.h"

class Plane {
public:
    Plane(float width, float height, int tileW, int tileH);
    ~Plane();

    float* getVertices();
    unsigned int* getIndices();

    size_t verticesSize();
    size_t indicesSize();

    void updateColors(double phi, double theta);

private:
    float width;
    float height;
    int tileW;
    int tileH;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices();
    void generateIndices();
};
