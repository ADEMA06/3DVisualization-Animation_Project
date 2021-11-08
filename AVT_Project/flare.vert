#version 330

uniform mat4 pvm;
uniform mat4 viewModel;
uniform mat3 normal_m;


uniform vec4 l_pos;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataOut;

void main () {

	vec4 pos = viewModel * position;

	DataOut.normal = normalize(normal_m * normal.xyz);
	DataOut.lightDir = vec3(l_pos - pos);
	DataOut.eye = vec3(-pos);
	DataOut.tex_coord = texCoord.st;

	gl_Position = pvm * position;	
}