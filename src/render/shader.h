#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertex_path, const char* fragment_path);
    ~Shader();
    void use();
    
    void uniformVec4(const char* form_name, float rx, float gy, float bz, float aw);
    void uniformi1(const char* form_name, int data);
    void uniformmat4fv(const char* form_name, const glm::mat4& mat);
private:
    GLuint m_shader_program;
};