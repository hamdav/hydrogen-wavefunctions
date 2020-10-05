#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./glm/gtc/type_ptr.hpp"

#include "./glad.h"


class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use();
    void setInt(const std::string &name, int value) const;
    void setBool(const std::string &name, bool value) const;
    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string &name, glm::mat4 mat) const;

private:

    void checkCompileErrors(unsigned int shader, std::string type);
};
