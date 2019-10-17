#version 430

layout(location = 0) in vec3 pos;		// Model-space position
layout(location = 1) in vec3 norm;		// Model-space normal
layout(location = 2) in vec2 cols;
layout(location = 4) uniform float amp1;
layout(location = 5) uniform float amp2;
layout(location = 6) uniform float amp3;


layout(location = 3)uniform float time;
smooth out vec3 fragNorm;	// Model-space interpolated normal
smooth out vec2 UV;
uniform mat4 xform;			// Model-to-clip space transform


vec3 dir =normalize(vec3(1.0,1.0,0.25));
float l = 0.9;
float pi = 3.142;
float w = sqrt(9.8*2*pi/l);

void main() {
	// Transform vertex position
	float Amp = amp1;
	float Amp2 = amp2;
	float Amp3 = amp3;
	float Qa =	1/(w*Amp);
	float Qb = 1/(w*Amp2);
	float Qc = 1/(w*Amp3);
	vec3 wave1;
	vec3 wave2;
	vec3 wave3;
	vec3 wave;
	vec3 wn1;
	vec3 wn2;
	vec3 wn3;
	float phase = 0.5;

	wave1.x = (Qa*Amp*dir.x*cos(dot(w*dir,pos)+phase*time));
	wave1.y = -(Qa*Amp*dir.y*cos(dot(w*dir,pos)+phase*time));
	wave1.z = Amp*sin(dot(w*dir,pos)+phase*time);
	
	wave2.x = (Qb*Amp2*dir.x*cos(dot(w*dir,pos)+phase*time));
	wave2.y = -(Qb*Amp2*dir.y*cos(dot(w*dir,pos)+phase*time));
	wave2.z = Amp2*sin(dot(w*dir,pos)+phase*time);
	
	wave3.x = (Qc*Amp3*dir.x*cos(dot(w*dir,pos)+phase*time));
	wave3.y = -(Qc*Amp3*dir.y*cos(dot(w*dir,pos)+phase*time));
	wave3.z = Amp3*sin(dot(w*dir,pos)+phase*time);
	wave = wave1 + wave2 + wave3;
	
	//wn1 =	dir.x*w*Amp*cos(dot(w*dir.xy,pos.xz)+5*time);
	gl_Position = xform * vec4(pos+wave, 10.0);
	UV = cols;
	// Interpolate normals
	fragNorm = norm;
}
