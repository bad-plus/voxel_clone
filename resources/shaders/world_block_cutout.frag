#version 330 core

in vec2 TexCoord;
in float vLight;
in float vAO;

out vec4 color;

uniform sampler2D ourTexture1;

void main() {
	float aoFactor = (vAO / 3.0);
	aoFactor = 0.4 + 0.6 * aoFactor; 
	vec4 tex = texture(ourTexture1, TexCoord);
	if(tex.a < 0.5)
		discard;
    vec4 pre_color = tex;
	color = pre_color * aoFactor;
}