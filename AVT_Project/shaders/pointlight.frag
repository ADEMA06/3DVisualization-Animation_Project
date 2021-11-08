#version 330

out vec4 colorOut;
uniform sampler2D texmap0;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;

uniform sampler2D texmap9;
uniform int pause_on;

uniform int texMode;

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


struct LightAttr {
	vec4 spec;
	vec4 diffuse;
	float intensity;
};



LightAttr spotLighting(LightAttr splight, vec3 normal, vec3 spot_l, vec3 spot_dir, float cutOff, vec3 eye) {
	
	splight.intensity = 0.0f;
	splight.intensity = max(dot(normal, spot_l),0.0);
	if(spot_on != 0 && pause_on == 0) {
		if(dot(spot_dir, spot_l) > spotlights[0].cutOff) {
			if (splight.intensity > 0.0) {
				vec3 h = normalize(spot_l + eye);
				float intSpec = max(dot(h,normal), 0.0);
				splight.spec += mat.specular * pow(intSpec, mat.shininess);
				splight.diffuse += mat.diffuse * splight.intensity;
			}
		}
	}

	return splight;
}

LightAttr pointLighting(LightAttr splight, vec3 normal, vec3 lightDir, vec3 eye) {
	
	if(point_on != 0 && pause_on == 0) {
			float distance = sqrt(pow(lightDir.x,2) + pow(lightDir.y,2) + pow(lightDir.z,2));
			float attenuation = 1.0/(1.0 + 0.1*distance+ 0.01*distance*distance);
			vec3 l = normalize(lightDir);
			splight.intensity += max(dot(normal,l), 0.0) * attenuation;
			if (splight.intensity > 0.0) {
				vec3 h = normalize(l + eye);
				float intSpec = max(dot(h,normal), 0.0);
				splight.spec += mat.specular * pow(intSpec, mat.shininess)* attenuation;
				splight.diffuse += mat.diffuse * splight.intensity;
			}
		}

	return splight;
}

LightAttr dirLighting(LightAttr light, vec3 normal, vec3 dir, vec3 eye) {
	float dirIntensity = 0.0f;
	if(dir_on != 0) {
		dirIntensity = max(dot(normal, dir), 0.0);
		if(dirIntensity > 0.0) {
			vec3 h = normalize(dir + eye);
			float intSpec = max(dot(h,normal), 0.0);
			light.spec += mat.specular * pow(intSpec, mat.shininess);

			light.diffuse += mat.diffuse * dirIntensity;
		}
	}

	return light;
}



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

	LightAttr light;
	light.spec = vec4(0.0);
	light.diffuse = vec4(0.0);

	light = dirLighting(light, n, dir_l, e);

	light = spotLighting(light, n, spot_l1, spot_dir1, spotlights[0].cutOff, e);

	light = spotLighting(light, n, spot_l2, spot_dir2, spotlights[1].cutOff, e);

	light.intensity = 0;
	for(int i = 0; i < 6; i = i+1) {
		light = pointLighting(light, n, pointlights[i].lightDir, e);
	}

	float dist = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
	float f = exp(-0.1*dist);


	if(mat.texCount == 0){
		colorOut = (light.diffuse + light.spec) + mat.ambient;	
	}
	else if(mat.texCount == 1){
		texel = texture(texmap0, DataIn.tex_coord);
		colorOut = (light.diffuse + light.spec) * texel  + mat.ambient;
	}
	else if(mat.texCount == 2){
		texel = texture(texmap2, DataIn.tex_coord);
		colorOut = (light.diffuse + light.spec) * texel  + mat.ambient;
	}
	else if(mat.texCount == 3){
		texel = texture(texmap2, DataIn.tex_coord);
		texel1 = texture(texmap0, DataIn.tex_coord);
		colorOut = (light.diffuse + light.spec) *  texel1 * texel  + mat.ambient;
	}
	else if(mat.texCount == 4){
		texel = texture(texmap0, DataIn.tex_coord);
		colorOut = (light.diffuse + light.spec) * texel + mat.ambient;
	}

else if(mat.texCount == 4){
		texel = texture(texmap3, DataIn.tex_coord);
		if((texel.a <= 0.05)  || (mat.diffuse.a == 0)) discard;
		else
			colorOut = mat.diffuse * texel;
	}
  
	else if(mat.texCount == 9 && texMode == 1){
		texel = texture(texmap9, DataIn.tex_coord);
		colorOut = (light.diffuse + light.spec) * texel + mat.ambient;
		
		if(texel.a == 0.0) discard;
		else { //FIGURE THIS OUT
			vec3 c = vec3(max(light.intensity*texel.rgb + spec.rgb, 0.1*texel.rgb));
			colorOut = vec4(vec3(colorOut), texel.a);
		}
			
	}

	if(diffMapCount != 0 && diffMapCount == 1)
		colorOut = (light.diffuse + light.spec) * texture(texUnitDiff, DataIn.tex_coord);

	

	vec3 colorRGB = vec3(colorOut);
	vec3 fogColor = vec3(0.75, 0.75, 0.75);
	//vec3 finalColor = mix(fogColor, colorRGB, f);
	colorOut = vec4(vec3(colorOut), mat.diffuse.a);
	//colorOut = vec4(vec3(finalColor), mat.diffuse.a);

	if(pause_on == 1) colorOut = vec4(vec3(colorOut)/3, colorOut.a);
}