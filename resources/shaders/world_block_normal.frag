#version 330 core

in vec2 TexCoord;
in float vLight;
in float vAO;

out vec4 color;

uniform sampler2D ourTexture1;

void main() {
	float aoFactor = (vAO / 3.0);
	aoFactor = 0.4 + 0.6 * aoFactor; 
    vec4 pre_color = texture(ourTexture1, TexCoord);
	
	color = pre_color * aoFactor;
}