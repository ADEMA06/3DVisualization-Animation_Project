#version 330
/*float res[24];
for (int i = 0; i < 24; i+=4) {
	float mult[4];
	float values[4] = { lightPos[i],lightPos[i + 1], lightPos[i + 2], lightPos[i + 3]};
	multMatrixPoint(VIEW, values, mult);
	res[i] = mult[0];
	res[i+1] = mult[1];
	res[i+2] = mult[2];
	res[i+3] = mult[3];
}
//multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
glUniform4fv(lPos_uniformId, 6, res);*/

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

uniform vec4 l_pos[6];

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir[6];
} DataOut;

void main () {

	vec4 pos = m_viewModel * position;

	DataOut.normal = normalize(m_normal * normal.xyz);
	for(int i = 0; i < 6; i++) {
		DataOut.lightDir[i] = vec3(l_pos[i] - pos);
	}

	DataOut.eye = vec3(-pos);

	gl_Position = m_pvm * position;	
}
