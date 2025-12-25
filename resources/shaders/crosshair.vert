#version 330 core
layout (location = 0) in vec3 position;

uniform float uScale;
uniform float uAspect;

void main() {
    gl_Position = vec4(position.x * uScale / uAspect, position.y * uScale, position.z, 1.0);
}