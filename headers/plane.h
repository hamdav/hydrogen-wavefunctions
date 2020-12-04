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

    int getn();
    int getl();
    int getm();

    void increment_n();
    void increment_l();
    void increment_m();
    void decrement_n();
    void decrement_l();
    void decrement_m();

    void zoomIn();
    void zoomOut();

    void incSensitivity();
    void decSensitivity();

    void updateColors(double phi, double theta);

private:
    float width;
    float height;
    int tileW;
    int tileH;
    int n;
    int l;
    int m;
    float awidth;
    float aheight;
    double norm_const;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices();
    void generateIndices();
};
