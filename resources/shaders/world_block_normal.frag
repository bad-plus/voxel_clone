#version 330 core

in vec2 TexCoord;
flat in uint vLight;
flat in uint vAO;

out vec4 color;

uniform sampler2D ourTexture1;

void main() {
    color = texture(ourTexture1, TexCoord);
}