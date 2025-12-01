#version 330 core

layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;
layout (location = 1) in vec2 texCoord;

//out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;
//out vec4 vertexColor;

void main() {
    gl_Position = transform * vec4(position, 1.0);
    //ourColor = color;
    TexCoord = texCoord;
    //vertexColor = vec4(1.0f, 0.3f, 0.3f, 1.0f);
}