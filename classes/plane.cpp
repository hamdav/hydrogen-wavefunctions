#include "../headers/plane.h"

Plane::Plane(float width, float height, int tileW, int tileH) {
    /* tileW and tileH are specified in points, not squares */
    this->width = width;
    this->height = height;
    this->tileW = tileW;
    this->tileH = tileH;

    generateVertices();
    generateIndices();
}

Plane::~Plane() {
}

float* Plane::getVertices() {
    return vertices.data();
}

unsigned int* Plane::getIndices() {
    return indices.data();
}

size_t Plane::verticesSize() {
    return vertices.size() * sizeof(float);
}

size_t Plane::indicesSize() {
    return indices.size() * sizeof(unsigned int);
}

void Plane::updateColors(double phi, double theta) {
    //double* colors = get_colors(4, 3, 1, phi, theta, -3e-9, 3e-9, -3e-9, 3e-9, tileW, tileH);
    double* colors = get_colors2_electric_boogaloo(4, 1, 0, phi, theta, -3e-9, 3e-9, -3e-9, 3e-9, 3e-9, tileW, tileH, 40);

    for ( int y = 0; y < tileH; y++ ) {
        for ( int x = 0; x < tileW * 6; x += 6 ) {
            vertices[y*tileW*6 + x + 3] = (float)colors[y*tileW*4 + x/6*4];
            vertices[y*tileW*6 + x + 4] = (float)colors[y*tileW*4 + x/6*4 + 1];
            vertices[y*tileW*6 + x + 5] = (float)colors[y*tileW*4 + x/6*4 + 2];
        }
    }

    delete colors;
}

void Plane::generateVertices() {
    vertices.resize(tileW*tileH*3*2);

    double* colors = get_colors(4, 3, 1, 0, glm::radians(45.0f), -3e-9, 3e-9, -3e-9, 3e-9, tileW, tileH);
    
    float xGap = ((float) width)/((float) tileW);
    float yGap = ((float) height)/((float) tileH);

    for ( int y = 0; y < tileH; y++ ) {
        for ( int x = 0; x < tileW * 6; x += 6 ) {
            vertices[y*tileW*6 + x]     = (x+3)/6.0f * xGap - width / 2.0f;
            vertices[y*tileW*6 + x + 1] = (y+0.25) * yGap - height / 2.0f;
            vertices[y*tileW*6 + x + 2] = 0.0f;
            vertices[y*tileW*6 + x + 3] = (float)colors[y*tileW*4 + x/6*4];
            vertices[y*tileW*6 + x + 4] = (float)colors[y*tileW*4 + x/6*4 + 1];
            vertices[y*tileW*6 + x + 5] = (float)colors[y*tileW*4 + x/6*4 + 2];
        }
    }

    delete colors;

}

void Plane::generateIndices() {
    // n-1 quads per n verts in x- and y-direction, two tris per quad, 3 points per tri
    indices.resize((tileW-1)*(tileH-1)*2*3);

    int j = 0;
    for ( int y = 0; y < tileH - 1; y++ ) {
        for ( int x = 0; x < tileW - 1; x++ ) {
            indices[j]     = y*tileW + x;
            indices[j + 1] = (y + 1)*tileW + x;
            indices[j + 2] = y*tileW + x + 1;

            indices[j + 3] = y*tileW + x + 1;
            indices[j + 4] = (y + 1)*tileW + x;
            indices[j + 5] = (y + 1)*tileW + x + 1;

            j += 6;
        }
    }
}

