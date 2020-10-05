#version 330 core
out vec4 FragColor;
in vec3 ourColor;
uniform float greenColor = 1.0;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
