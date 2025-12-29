#version 330 core
in vec2 TexCoord;
flat in uint vLight;
in float vAO;
out vec4 color;
uniform sampler2D ourTexture1;
uniform vec3 sunColor;

void main() {
	uint r = (vLight      ) & 0xFu;
	uint g = (vLight >> 4 ) & 0xFu;
	uint b = (vLight >> 8 ) & 0xFu;
	uint s = (vLight >> 12) & 0xFu;
	
	vec3 blockLight = vec3(float(r), float(g), float(b)) / 15.0;
	float sunIntensity = float(s) / 15.0;
	vec3 sunLight = sunColor * sunIntensity;
	
	float aoFactor = mix(0.6, 1.0, vAO / 3.0);
	vec3 finalLight = blockLight + sunLight * aoFactor;
	
	vec4 tex = texture(ourTexture1, TexCoord);
	
	color = tex * vec4(finalLight, 1.0);
}