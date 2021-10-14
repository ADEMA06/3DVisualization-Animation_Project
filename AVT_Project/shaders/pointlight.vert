#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;


in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

uniform vec4 spot_dir;

struct DirectionalLight {
	vec4 direction;
	bool on;
};

struct SpotLight {
	vec4 position;
	vec4 direction;
	bool on;
	float cutOff;
};

struct pointLight{
	vec4 position;
	float on;
	vec3 lightDir;
};

uniform pointLight uni_pointlights[6];
uniform DirectionalLight uni_dirlight;
uniform SpotLight uni_spotlights;

out pointLight pointlights[6];
out DirectionalLight dirlight;
out SpotLight spotlights;



out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataOut;

void main () {

	vec4 pos = m_viewModel * position;

	DataOut.normal = normalize(m_normal * normal.xyz);
	DataOut.lightDir = vec3(uni_spotlights.position - pos);
	DataOut.eye = vec3(-pos);
	DataOut.tex_coord = texCoord.xy;

	dirlight.direction = -uni_dirlight.direction;
	dirlight.on = uni_dirlight.on;

	spotlights.direction = -uni_spotlights.direction;
	spotlights.position = uni_spotlights.position;
	spotlights.on = uni_spotlights.on;
	spotlights.cutOff = uni_spotlights.cutOff;

	for(int i = 0; i < 6; i++) {
		pointlights[i].lightDir = vec3(uni_pointlights[i].position - pos);
		pointlights[i].on = uni_pointlights[i].on;
	}

	gl_Position = m_pvm * position;	
}