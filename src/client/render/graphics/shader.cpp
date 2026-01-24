#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../../core/logger.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

std::string readFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if(!file) {
        LOG_ERROR("Error read file: {0}", file_path);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

Shader::Shader(const char* vertex_path, const char* fragment_path) {
    const std::string shader_base_path = std::string(std::string(RESOURCES_PATH) + "shaders/");

    std::string vertex_code = readFile(shader_base_path + vertex_path);
    std::string fragment_code = readFile(shader_base_path + fragment_path);

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    m_shader_program = glCreateProgram();

    GLint success;
    GLchar info_log[512];

    if(vertex_code.length() > 0) {
        GLuint vertex_shader;
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
            LOG_ERROR("Shader {0} not compiled:\n\t{1}", vertex_path, info_log);
        } else glAttachShader(m_shader_program, vertex_shader);

        glDeleteShader(vertex_shader);
    }

    if(fragment_code.length() > 0) {
        GLuint fragment_shader;
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
            LOG_ERROR("Shader {0} not compiled:\n\t{1}", fragment_path, info_log);
        } else glAttachShader(m_shader_program, fragment_shader);

        glDeleteShader(fragment_shader);
    }

    glLinkProgram(m_shader_program);
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(m_shader_program, 512, nullptr, info_log);
        LOG_ERROR("Shader program not compiled\n\t{0}", info_log);
    }
}

Shader::~Shader() {
    glDeleteProgram(m_shader_program);
}

void Shader::use() const {
    glUseProgram(m_shader_program);
}

void Shader::uniformVec4(const char* form_name, float rx, float gy, float bz, float aw) const {
    GLint form_location = glGetUniformLocation(m_shader_program, form_name);
    if(form_location == -1) {
        LOG_WARN("bad request to update shader uniform ({0})", form_name);
        return;
    }
    glUniform4f(form_location, rx, gy, bz, aw);
}

void Shader::uniformVec3(const char* form_name, float rx, float gy, float bz) const {
	GLint form_location = glGetUniformLocation(m_shader_program, form_name);
	if (form_location == -1) {
		LOG_WARN("bad request to update shader uniform ({0})", form_name);
		return;
	}
	glUniform3f(form_location, rx, gy, bz);
}

void Shader::uniformi1(const char* form_name, int data) const {
    GLint form_location = glGetUniformLocation(m_shader_program, form_name);
    if(form_location == -1) {
        LOG_WARN("bad request to update shader uniform ({0})", form_name);
        return;
    }
    glUniform1i(form_location, data);
}

void Shader::uniform1f(const char* form_name, float data) const {
	GLint form_location = glGetUniformLocation(m_shader_program, form_name);
	if (form_location == -1) {
		LOG_WARN("bad request to update shader uniform ({0})", form_name);
		return;
	}
	glUniform1f(form_location, data);
}

void Shader::uniformmat4fv(const char* form_name, const glm::mat4& mat) const {
    GLint form_location = glGetUniformLocation(m_shader_program, form_name);
    if(form_location == -1) {
        LOG_WARN("bad request to update shader uniform ({0})", form_name);
        return;
    }
    glUniformMatrix4fv(form_location, 1, GL_FALSE, glm::value_ptr(mat));
}