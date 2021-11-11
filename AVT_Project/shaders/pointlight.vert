#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;
uniform mat4 m_Model;   //por causa do cubo para a skybox
uniform mat4 m_View;



uniform int instanced;
uniform vec3 offsets[908];
uniform int texMode;


in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;


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
	vec3 reflected;
	vec2 sphere_coord;
	vec3 skyboxTexCoord;
} DataOut;

void main () {

	vec4 new_pos = position;

	vec3 n, t, b;
	vec3 eyeDir;
	vec3 aux;

	if(instanced == 1) {
		new_pos = vec4(vec3(position) + vec3(offsets[gl_InstanceID]), 1.0f);
	}
	pos = m_viewModel * new_pos;

	eyeDir = vec3(-pos);

	dirlight.direction = -uni_dirlight.direction;

	n = normalize(m_normal * normal.xyz);

	if(texMode == 2)  {  //convert eye and light vectors to tangent space

		//Calculate components of TBN basis in eye space
		t = normalize(m_normal * tangent.xyz);  
		b = tangent.w * cross(n,t);

		aux.x = dot(dirlight.direction.xyz, t);
		aux.y = dot(dirlight.direction.xyz, b);
		aux.z = dot(dirlight.direction.xyz, n);
		dirlight.direction = vec4(normalize(aux), 0.0f);

		aux.x = dot(eyeDir, t);
		aux.y = dot(eyeDir, b);
		aux.z = dot(eyeDir, n);
		eyeDir = normalize(aux);
	}

	DataOut.normal = n;
	//DataOut.lightDir = vec3(uni_spotlights.position - pos);
	DataOut.eye = eyeDir;
	DataOut.tex_coord = texCoord.st;
	DataOut.skyboxTexCoord = vec3(m_Model * position);	//Transformação de modelação do cubo unitário 
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x


	spotlights[0].direction = -uni_spotlights[0].direction;
	spotlights[0].position = uni_spotlights[0].position;
	spotlights[0].cutOff = uni_spotlights[0].cutOff;
	spotlights[0].light_dir = vec3(uni_spotlights[0].position-pos);

	spotlights[1].direction = -uni_spotlights[1].direction;
	spotlights[1].position = uni_spotlights[1].position;
	spotlights[1].cutOff = uni_spotlights[1].cutOff;
	spotlights[1].light_dir = vec3(uni_spotlights[1].position-pos);

	if(texMode == 3){
		DataOut.reflected = vec3 (transpose(m_View) * vec4 (vec3(reflect(-DataOut.eye, DataOut.normal)), 0.0)); //reflection vector in world coord
		DataOut.reflected.x= -DataOut.reflected.x; // as texturas foram mapeadas no interior da skybox 
	}

	for(int i = 0; i < 6; i++) {
		pointlights[i].lightDir = vec3(uni_pointlights[i].position - pos);
	}

	gl_Position = m_pvm * new_pos;	
}