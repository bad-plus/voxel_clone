#version 330 core

//in vec4 vertexColor;

//in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

//uniform vec4 ourColor;

uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;

void main() {
    //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture1, TexCoord), 0.0f);
}