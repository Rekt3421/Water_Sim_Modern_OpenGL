#version 330

smooth in vec3 fragNorm;	// Interpolated model-space normal
in vec2 UV;
uniform sampler2D diffuse_tex;
out vec4 outCol;	// Final pixel color

void main() {
	// Visualize normals as colors
	outCol =  texture(diffuse_tex, UV);
	outCol.w = 0.01;
}