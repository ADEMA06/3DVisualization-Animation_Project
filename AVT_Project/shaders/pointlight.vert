#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

uniform int instanced;
uniform vec3 offsets[908];


in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

uniform vec4 spot_dir;

struct DirectionalLight {
	vec4 direction;
};

struct SpotLight {
	vec4 position;
	vec4 direction;
	vec3 light_dir;
	float cutOff;
};

struct pointLight{
	vec4 position;
	vec3 lightDir;
};

uniform pointLight uni_pointlights[6];
uniform DirectionalLight uni_dirlight;
uniform SpotLight uni_spotlights[2];

out pointLight pointlights[6];
out DirectionalLight dirlight;
out SpotLight spotlights[2];

out vec4 pos;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataOut;

void main () {

	vec4 new_pos = position;

	if(instanced == 1) {
		new_pos = vec4(vec3(position) + vec3(offsets[gl_InstanceID]), 1.0f);
	}
	pos = m_viewModel * new_pos;

	DataOut.normal = normalize(m_normal * normal.xyz);
	//DataOut.lightDir = vec3(uni_spotlights.position - pos);
	DataOut.eye = vec3(-pos);
	DataOut.tex_coord = texCoord.st;

	dirlight.direction = -uni_dirlight.direction;

	spotlights[0].direction = -uni_spotlights[0].direction;
	spotlights[0].position = uni_spotlights[0].position;
	spotlights[0].cutOff = uni_spotlights[0].cutOff;
	spotlights[0].light_dir = vec3(uni_spotlights[0].position-pos);

	spotlights[1].direction = -uni_spotlights[1].direction;
	spotlights[1].position = uni_spotlights[1].position;
	spotlights[1].cutOff = uni_spotlights[1].cutOff;
	spotlights[1].light_dir = vec3(uni_spotlights[1].position-pos);

	for(int i = 0; i < 6; i++) {
		pointlights[i].lightDir = vec3(uni_pointlights[i].position - pos);
	}

	gl_Position = m_pvm * new_pos;	
}