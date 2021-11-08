#ifndef __TABLE_H__
#define __TABLE_H__

#include <string>
#include <fstream>
#include "MeshBuilder.h"
#include "AssimpMesh.h"
#include "StaticObject.h"

extern char model_dir[50];

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  1500

class Table : public StaticObject {
	//Meshes
	struct MyMesh base;
	std::vector<struct MyMesh> legs;
	std::vector<struct MyMesh> meshes;
	std::vector<struct MyMesh> pyramid;
	const aiScene* tableScene = NULL;
	const aiScene* pyramidScene = NULL;
	struct MyMesh particle;
	int scenery = 0;

	//Dimensions
	float width;
	float height;
	float thickness;
	float legSide;
	float legHeight;

	typedef struct {
		float	life;		// vida
		float	fade;		// fade
		float	r, g, b;    // color
		GLfloat x, y, z;
		GLfloat vx, vy, vz; // velocidade 
		GLfloat ax, ay, az; 
	} Particle;

	Particle particles[MAX_PARTICULAS];
	int dead_num_particles = 0;

	bool volcanoErupting = false;


public:

	Table(float width, float height, float thickness, float legSide, float legHeight, vec3 position) : StaticObject(position) {
		this->height = height;
		this->width = width;
		this->thickness = thickness;
		this->legSide = legSide;
		this->legHeight = legHeight;
	}

	void chengeScenery() {
		scenery += 1;
	}

	void createTable(GLuint* textures, int offset) {
		MeshBuilder builder;

		float amb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 500.0f;
		int texcount = 3;
		vec3 table_pos = getPosition();
		base = createCube();
		base = builder.setMesh(base, amb, diff, spec, emissive, shininess, texcount, table_pos);
		particle = createQuad(1, 1);
		particle.mat.texCount = 4;
		MyMesh amesh;
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				amesh = createCube();
				vec3 position = table_pos + vec3(j * width / 2 - j * legSide / 2, -legHeight, i * height / 2 - i * legSide / 2);
				amesh = builder.setMesh(amesh, amb, diff, spec, emissive, shininess, texcount, position);
				legs.push_back(amesh);
			}
		}
		std::string filepath;
		strcpy(model_dir, "Volcano");
		while (true) {

			std::ostringstream oss;
			oss << model_dir << "/" << model_dir << ".obj";;
			filepath = oss.str();   //path of OBJ file in the VS project

			strcat(model_dir, "/");
			//check if file exists
			std::ifstream fin(filepath.c_str());
			if (!fin.fail()) {
				fin.close();
				break;
			}
			else
				printf("Couldn't open file: %s\n", filepath.c_str());
		}
		tableScene = Import3DFromFile(filepath, tableScene, &vec3(), &vec3());
		meshes = createMeshFromAssimp(tableScene, textures, offset);
		strcpy(model_dir, "pyramid");
		while (true) {

			std::ostringstream oss;
			oss << model_dir << "/" << model_dir << ".obj";;
			filepath = oss.str();   //path of OBJ file in the VS project

			strcat(model_dir, "/");
			//check if file exists
			std::ifstream fin(filepath.c_str());
			if (!fin.fail()) {
				fin.close();
				break;
			}
			else
				printf("Couldn't open file: %s\n", filepath.c_str());
		}
		pyramidScene = Import3DFromFile(filepath, pyramidScene, &vec3(), &vec3());
		pyramid = createMeshFromAssimp(pyramidScene, textures, offset);
	}

	void tableRecursiveDraw(aiNode* nd, VSShaderLib* shader, GLuint* textures, int offset, std::vector<struct MyMesh> meshes) {
		MeshBuilder builder;
		GLint diffMapCount_loc = glGetUniformLocation(shader->getProgramIndex(), "diffMapCount");
		for (int n = 0; n < meshes.size(); ++n) {
			int diffMapCount = 0;
			glUniform1ui(diffMapCount_loc, 0);
			builder.setShaders(shader, meshes[n]);
			if (meshes[n].mat.texCount != 0) {
				for (unsigned int i = 0; i < meshes[n].mat.texCount; ++i) {
					if (meshes[n].texTypes[i] == DIFFUSE) {
						if (diffMapCount == 0) {
							diffMapCount++;
							glActiveTexture(GL_TEXTURE0 + offset);
							glBindTexture(GL_TEXTURE_2D, textures[offset]);
							GLint loc = glGetUniformLocation(shader->getProgramIndex(), "texUnitDiff");
							glUniform1i(loc, meshes[n].texUnits[i]);
							glUniform1i(diffMapCount_loc, diffMapCount);
						}
					}
				}
			}
			builder.drawMesh(meshes[n], shader);
		}
	}

	void drawTable(VSShaderLib* shader, GLuint* textures, int offset) {
		MeshBuilder builder;
		builder.setShaders(shader, base);
		pushMatrix(MODEL);
		translate(MODEL, -width / 2, -thickness, -height / 2);
		translate(MODEL, getPosition().x, getPosition().y, getPosition().z);
		scale(MODEL, width, thickness, height);
		builder.drawMesh(base, shader);
		popMatrix(MODEL);

		pushMatrix(MODEL);
		translate(MODEL, width / 3, 0.0f, width / 3);
		if (scenery == 0) {
			scale(MODEL, 10.0f, 10.0f, 10.0f);
			tableRecursiveDraw(tableScene->mRootNode, shader, textures, offset, meshes);
		}
		else {
			scale(MODEL, 0.7f, 0.7f, 0.7f);
			tableRecursiveDraw(tableScene->mRootNode, shader, textures, offset, pyramid);
		}
		popMatrix(MODEL);


		for (int i = 0; i < legs.size(); i++) {
			builder.setShaders(shader, legs.at(i));
			pushMatrix(MODEL);
			translate(MODEL, -legSide / 2, -thickness, -legSide / 2);
			translate(MODEL, legs.at(i).position.x, legs.at(i).position.y, legs.at(i).position.z);
			scale(MODEL, legSide, legHeight, legSide);
			builder.drawMesh(legs.at(i), shader);
			popMatrix(MODEL);
		}

		if (volcanoErupting) {

			updateParticles();

			glDisable(GL_CULL_FACE);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures[3]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);  //Depth Buffer Read Only
			GLint loc = glGetUniformLocation(shader->getProgramIndex(), "texmap3");
			glUniform1i(loc, 3); // draw modulated textured particles 

			loc = glGetUniformLocation(shader->getProgramIndex(), "mat.texCount");
			glUniform1i(loc, particle.mat.texCount);
			float particle_color[4];

			for (int i = 0; i < MAX_PARTICULAS; i++)
			{
				if (particles[i].life > 0.0f)
				{

					particle_color[0] = particles[i].r;
					particle_color[1] = particles[i].g;
					particle_color[2] = particles[i].b;
					particle_color[3] = particles[i].life;

					// send the material - diffuse color modulated with texture
					loc = glGetUniformLocation(shader->getProgramIndex(), "mat.diffuse");
					glUniform4fv(loc, 1, particle_color);

					pushMatrix(MODEL);
					translate(MODEL, width / 3, 0.0f, width / 3);
					translate(MODEL, particles[i].x, particles[i].y, particles[i].z);

					// send matrices to OGL
					computeDerivedMatrix(PROJ_VIEW_MODEL);
					glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
					glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
					computeNormalMatrix3x3();
					glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

					glBindVertexArray(particle.vao);
					glDrawElements(particle.type, particle.numIndexes, GL_UNSIGNED_INT, 0);
					popMatrix(MODEL);
				}
				else dead_num_particles++;
			}

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_TRUE); //make depth buffer again writeable

			if (dead_num_particles == MAX_PARTICULAS) {
				volcanoErupting = false;
				dead_num_particles = 0;
				printf("All particles dead\n");
			}
		}
	}

	void Erupt() {
		if (scenery == 0) {
			iniParticles();
			volcanoErupting = true;
		}
	}

	void iniParticles(void)
	{
		GLfloat v, theta, phi, v2;
		int i;

		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			v = 0.8 * frand() + 0.2;
			v2 = 0.4 * frand() + 0.2;
			phi = frand() * M_PI;
			theta = 2.0 * frand() * M_PI;

			particles[i].x = 0.0f;
			particles[i].y = 6.0f;
			particles[i].z = 0.0f;
			particles[i].vx = v * cos(theta) * sin(phi);
			particles[i].vy = v * sin(phi);
			particles[i].vz = v * sin(theta) * sin(phi);
			particles[i].ax = 0.0f; /* simular um pouco de vento */
			particles[i].ay = -0.05f; 
			particles[i].az = 0.0f;

			/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
			/*particles[i].r = 0.882f;
			particles[i].g = 0.552f;
			particles[i].b = 0.211f;*/
			
			particles[i].r = 1.0f;
			particles[i].g = 1.0f;
			particles[i].b = 1.0f;

			

			particles[i].life = 1.0f;		/* vida inicial */
			particles[i].fade = 0.0025f;	    
		}
	}

	void updateParticles()
	{
		int i;
		float h;

		//h = 0.125f;
		h = 0.033;
		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			particles[i].x += (h * particles[i].vx);
			particles[i].y += (h * particles[i].vy);
			particles[i].z += (h * particles[i].vz);
			particles[i].vx += (h * particles[i].ax);
			particles[i].vy += (h * particles[i].ay);
			particles[i].vz += (h * particles[i].az);
			particles[i].life -= particles[i].fade;
		}
	}
};

#endif