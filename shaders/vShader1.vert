#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 transform;

void main() {
    float height = exp(-(aPos.x*aPos.x + aPos.y*aPos.y)*100) * sin((aPos.x*aPos.x + aPos.y*aPos.y)*100);
    gl_Position = transform * vec4(aPos.x, aPos.y, -height, 1.0);
    ourColor = aColor.xyz * height + 0.1;
}
