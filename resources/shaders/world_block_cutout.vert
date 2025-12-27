#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aLight;
layout (location = 4) in uint aAO;

out vec2 TexCoord;
flat out uint vLight;
flat out uint vAO;

uniform mat4 transform;

void main() {
	vLight = aLight;
	vAO = aAO;

    gl_Position = transform * vec4(aPosition, 1.0);
    TexCoord = aTexCoord;
}