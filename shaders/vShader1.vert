#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 transform;

void main() {
    float r = sqrt(aPos.x*aPos.x + aPos.y*aPos.y)*50;
    gl_Position = transform * vec4(aPos.x, aPos.y, -(sin(r)*0.2)/r, 1.0);
    ourColor = aColor.xyz * sin(r)*1/(r+0.00001) + 0.4;
}
