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

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir[6];
} DataIn;

void main() {

	vec4 spec = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 l = vec3(0.0); 
	vec3 e = normalize(DataIn.eye);
	float intensity = 0.0;
	for(int i = 0; i < 6; i = i+1) {
		float distance = sqrt(pow(DataIn.lightDir[i].x,2) + pow(DataIn.lightDir[i].y,2) + pow(DataIn.lightDir[i].z,2));
		float attenuation = 1.0/(1.0 + 0.1*distance+ 0.01*distance*distance);
		l = normalize(DataIn.lightDir[i]);
		intensity += max(dot(n,l), 0.0) * attenuation;
		

		
		if (intensity > 0.0) {
			vec3 h = normalize(l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec += mat.specular * pow(intSpec, mat.shininess)* attenuation;
		}
		
		
	}
	colorOut = max(intensity * mat.diffuse + spec, mat.ambient);
	
}
