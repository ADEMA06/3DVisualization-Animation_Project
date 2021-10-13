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
};

struct SpotLight {
	vec4 position;
	vec4 direction;
	float cutOff;
};

struct pointLight{
	vec4 position;
	vec3 lightDir;
};

uniform int point_on;
uniform int dir_on;
uniform int spot_on;

in pointLight pointlights[6];
in DirectionalLight dirlight;
in SpotLight spotlights;

in vec4 pos;

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
	if(dir_on != 0) {
		dirIntensity = max(dot(n, dir_l), 0.0);
		if(dirIntensity > 0.0) {
			vec3 h = normalize(dir_l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec += mat.specular * pow(intSpec, mat.shininess);

			diffuse += mat.diffuse * dirIntensity;
		}
	}

	float spotIntensity = 0.0f;
	spotIntensity = max(dot(n, l), 0.0);
	if(spot_on != 0) {
		if(dot(spot_dir, l) > 0.866) {
			if (spotIntensity > 0.0) {
				vec3 h = normalize(l + e);
				float intSpec = max(dot(h,n), 0.0);
				spec += mat.specular * pow(intSpec, mat.shininess);
				diffuse += mat.diffuse * spotIntensity;
			}
		}
	}


	float pointIntensity = 0.0;
	for(int i = 0; i < 6; i = i+1) {
		if(point_on != 0) {
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
		
	}

	float dist = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
	float f = exp(-0.1*dist);

	colorOut = diffuse + spec + mat.ambient;
	vec3 colorRGB = vec3(colorOut);
	vec3 fogColor = vec3(0.75, 0.1, 0.1);
	vec3 finalColor = mix(fogColor, colorRGB, f);
	
	colorOut = vec4(finalColor, 1.0f);
	//colorOut = vec4(dist, dist, dist, 1.0f);

}