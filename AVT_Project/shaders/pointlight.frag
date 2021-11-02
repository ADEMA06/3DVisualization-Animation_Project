#version 330

out vec4 colorOut;
uniform sampler2D texmap0;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;

uniform int pause_on;

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
	vec3 light_dir;
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
in SpotLight spotlights[2];

in vec4 pos;


uniform int diffMapCount;
uniform	sampler2D texUnitDiff;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataIn;


void main() {

	vec4 texel = vec4(1.0); 
	vec4 texel1 = vec4(1.0);

	vec4 spec = vec4(0.0);
	vec4 diffuse = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 dir_l = normalize(vec3(dirlight.direction));
	vec3 l;
	vec3 spot_l1 = normalize(spotlights[0].light_dir);
	vec3 spot_l2 = normalize(spotlights[1].light_dir);
	vec3 spot_dir1 = normalize(vec3(spotlights[0].direction));
	vec3 spot_dir2 = normalize(vec3(spotlights[1].direction));
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
	spotIntensity = max(dot(n, spot_l1), 0.0);
	if(spot_on != 0 && pause_on == 0) {
		if(dot(spot_dir1, spot_l1) > spotlights[0].cutOff) {
			if (spotIntensity > 0.0) {
				vec3 h = normalize(spot_l1 + e);
				float intSpec = max(dot(h,n), 0.0);
				spec += mat.specular * pow(intSpec, mat.shininess);
				diffuse += mat.diffuse * spotIntensity;
			}
		}
	}

	float spotIntensity1 = 0.0f;
	spotIntensity1 = max(dot(n, spot_l2), 0.0);
	if(spot_on != 0 && pause_on == 0) {
		if(dot(spot_dir2, spot_l2) > spotlights[1].cutOff) {
			if (spotIntensity1 > 0.0) {
				vec3 h = normalize(spot_l2 + e);
				float intSpec = max(dot(h,n), 0.0);
				spec += mat.specular * pow(intSpec, mat.shininess);
				diffuse += mat.diffuse * spotIntensity1;
			}
		}
	}


	float pointIntensity = 0.0;
	for(int i = 0; i < 6; i = i+1) {
		if(point_on != 0 && pause_on == 0) {
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

	if(mat.texCount == 0){
		colorOut = (diffuse + spec) + mat.ambient;	
	}
	else if(mat.texCount == 1){
		texel = texture(texmap0, DataIn.tex_coord);
		colorOut = (diffuse + spec) * texel  + mat.ambient;
	}
	else if(mat.texCount == 2){
		texel = texture(texmap2, DataIn.tex_coord);
		colorOut = (diffuse + spec) * texel  + mat.ambient;
	}
	else if(mat.texCount == 3){
		texel = texture(texmap2, DataIn.tex_coord);
		texel1 = texture(texmap0, DataIn.tex_coord);
		colorOut = (diffuse + spec) *  texel1 * texel  + mat.ambient;
	}
	else if(mat.texCount == 4){
		texel = texture(texmap0, DataIn.tex_coord);
		colorOut = (diffuse + spec) * texel + mat.ambient;
	}

	if(diffMapCount != 0 && diffMapCount == 1)
		colorOut = (diffuse + spec) * texture(texUnitDiff, DataIn.tex_coord);

	

	vec3 colorRGB = vec3(colorOut);
	vec3 fogColor = vec3(0.75, 0.75, 0.75);
	vec3 finalColor = mix(fogColor, colorRGB, f);
	colorOut = vec4(vec3(colorOut), mat.diffuse.a);
	//colorOut = vec4(vec3(finalColor), mat.diffuse.a);

	if(pause_on == 1) colorOut = vec4(vec3(colorOut)/3, colorOut.a);
}