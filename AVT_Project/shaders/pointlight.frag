#version 330

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

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
	bool on;
	vec3 lightDir;
};

in pointLight pointlights[6];
in DirectionalLight dirlight;
in SpotLight spotlights;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn;


void main() {

	vec4 spec = vec4(0.0);
	vec4 diffuse = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 l = normalize(DataIn.lightDir);
	vec3 dir_l = normalize(vec3(dirlight.direction));
	vec3 spot_dir = normalize(vec3(spotlights.direction));
	vec3 e = normalize(DataIn.eye);

	float dirIntensity = 0.0f;
	if(dirlight.on) {
		dirIntensity = max(dot(n, dir_l), 0.0);
		if(dirIntensity > 0.0) {
			vec3 h = normalize(dir_l + e);
			float intSpec = max(dot(h,n), 0.0);
			intSpec = 2;
			spec += mat.specular * pow(intSpec, mat.shininess);

			diffuse += mat.diffuse * dirIntensity;
		}
	}

	float spotIntensity = 0.0f;
	spotIntensity = max(dot(n, l), 0.0);
	if(dot(spot_dir, l) > 0.866) {
		if (spotIntensity > 0.0) {
			vec3 h = normalize(l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec += mat.specular * pow(intSpec, mat.shininess);
			diffuse += mat.diffuse * spotIntensity;
		}
	}


	float pointIntensity = 0.0;
	for(int i = 0; i < 6; i = i+1) {
		float distance = sqrt(pow(pointlights[i].lightDir.x,2) + pow(pointlights[i].lightDir.y,2) + pow(pointlights[i].lightDir.z,2));
		float attenuation = 1.0/(1.0 + 0.1*distance+ 0.01*distance*distance);
		l = normalize(pointlights[i].lightDir);
		pointIntensity += max(dot(n,l), 0.0) * attenuation;
			
		if (pointIntensity > 0.0) {
			vec3 h = normalize(l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec += mat.specular * pow(intSpec, mat.shininess)* attenuation;
			diffuse += mat.diffuse * pointIntensity;
		}
	}

	colorOut = diffuse + spec + mat.ambient;//max((dirIntensity+spotIntensity+pointIntensity) * diffuse + spec, mat.ambient);

}