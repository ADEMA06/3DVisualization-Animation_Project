#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;


in vec4 position;
in vec4 normal;    //por causa do gerador de geometria

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
} DataOut;

void main () {

	pos = m_viewModel * position;

	DataOut.normal = normalize(m_normal * normal.xyz);
	//DataOut.lightDir = vec3(uni_spotlights.position - pos);
	DataOut.eye = vec3(-pos);

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

	gl_Position = m_pvm * position;	
}