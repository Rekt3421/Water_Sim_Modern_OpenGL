#version 430

layout(location = 0) in vec3 pos;		// Model-space position
layout(location = 1) in vec3 norm;		// Model-space normal
layout(location = 2) in vec2 tex_coord_attrib;
layout(location = 3) uniform float time;
layout(location = 4) uniform float degree;
//smooth out vec3 fragNorm;	// Model-space interpolated normal
 out vec2 tex_coord;

uniform mat4 xform;			// Model-to-clip space transform
uniform mat4 rotate;


void main() {
			
		float cosine = cos((0));
		float sine = sin((0));
		//gl_Position =  xform*vec4(pos.x*cosine+pos.y*sine,pos.x*sine-pos.y*cosine,pos.z,1);
		gl_Position =  xform*vec4(pos.x,pos.y+0.35,pos.z*cosine - pos.y*sine,5);
	//gl_Position =  xform*vec4(pos.x + degree,pos.x*sine+pos.y*cosine,pos.z,1);
	//gl_Position =  xform*vec4(pos.x*cosine-pos.z*sine,pos.y,pos.x*sine+pos.z*cosine,1);
	// Interpolate normals
//	fragNorm = norm;
	tex_coord= tex_coord_attrib;
}