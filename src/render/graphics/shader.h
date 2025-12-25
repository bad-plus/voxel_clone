#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertex_path, const char* fragment_path);
    ~Shader();
    void use() const;
    
    void uniformVec4(const char* form_name, float rx, float gy, float bz, float aw) const;
    void uniformi1(const char* form_name, int data) const;
    void uniform1f(const char* form_name, float data) const;
    void uniformmat4fv(const char* form_name, const glm::mat4& mat) const;
private:
    GLuint m_shader_program;
};