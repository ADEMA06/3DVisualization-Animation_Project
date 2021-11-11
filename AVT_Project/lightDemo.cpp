//
// AVT demo light 
// based on demos from GLSL Core Tutorial in Lighthouse3D.com   
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <random>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "Texture_Loader.h"
#include "geometry.h"
#include "Table.h"
#include "Car.h"
#include "Orange.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "Butter.h"
#include "Road.h"
#include "Light.h"
#include "Candle.h"
#include "HUD.h"
#include "BillBoardObject.h"
#include "flare.h"

using namespace std;

#define CAPTION "AVT Per Fragment Phong Lightning Demo"
int WindowHandle = 0;
int WinX = 640, WinY = 480;

unsigned int FrameCount = 0;

bool keys[256] = { false };

int pointLights_on = 1;
int t;

float camera_angle_xz = M_PI;

vec4 car_color(1.0f, 1.0f, 1.0f, 0.7f);
vec4 color_tire(0.1f, 0.1f, 0.1f, 1.0f);
vec4 cheerio_color(1.0f, 0.874f, 0.0f, 1.0f);
vec4 butter_foil_color(0.0f, 0.0f, 0.9f, 1.0f);

vec3 table_pos(0.0f, 0.0f, 0.0f);

vec3 car_pos(1.0f, 0.0f, 1.0f);
vec3 butter_pos(5.0f, 0.0f, 0.0f);


vec3 orange_pos(5.0f, 0.0f, 5.0f);

BillBoardObject bb(vec3(40.0, 0.0, -30.0), 8);

Table table(100.0f, 100.0f, 0.8f, 0.5f, 10.0f, table_pos);
Car car(car_pos, 2.5f, 20.0f, car_color, color_tire);
Butter butter(butter_pos, butter_foil_color);
Road road(vec3(0.0f, 0.0f, 0.0f));
std::vector<Cheerio> cheerios;
std::vector<Orange> oranges;

vector<Candle> candles;

textToRender* lives;
textToRender* pause;
textToRender* points;
textToRender* game_over;

bool isGameOver;

int current_camera = 0;
Camera* cameras[5];

VSShaderLib* shader = new VSShaderLib();
VSShaderLib* shaderText = new VSShaderLib();

//File with the font
const string font_name = "fonts/arial.ttf";

HUD hud;

FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates

//Vector with meshes
vector<struct MyMesh> myMeshes;

struct MyMesh helpCube;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint model_uniformId;
GLint dir_light_uniformId;
GLint pause_on_Id;
GLint tex_loc0, tex_loc1, tex_loc2, tex_loc3, tex_loc8; 
GLint tex_cube_loc, tex_normalMap_loc;
GLuint TextureArray[16];
GLint texMode_uniformId, shadowMode_uniformId;


// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 5.0f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];
float lightPos[4] = { 4.0f, 6.0f, 2.0f, 1.0f };

float oldTime = 0.0f;
float dt = 0.0f;

bool leftKey = false;
bool isPassingFlag = false;


//----------------Lights---------------------
Light directionalLight;
//-------------------------------------------

bool mouse_pressed = false;
vec3 oldPosition = vec3(0.0f, 0.0f, 0.0f);
vec3 currentPosition;

void resetGame();


void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	cameras[current_camera]->setViewPort(w, h);
	WinX = w;
	WinY = h;
	lives->coordinates = { WinX / 10.0f, WinY / 10.0f };
	pause->coordinates = { (WinX / 2.0f) - 75.0f, WinY / 2.0f };
	points->coordinates = { WinX / 10.0f, WinY * 9.0f / 10.0f };
	game_over->coordinates = { (WinX / 2.0f) - 100.0f, WinY / 2.0f };
}

void pauseObjects() {
	car.setPause(keys['s']);
	butter.setPause(keys['s']);
	for (Orange& o : oranges) {
		o.setPause(keys['s']);
	}

	for (Cheerio* c : road.getLimits()) {
		c->setPause(keys['s']);
	}

}

void update() {
	if (!car.getPause() && !isGameOver) {
		if (!(keys['q'] || keys['a'])) {
			car.stop(dt);
		}
		if (keys['q']) {
			car.goForward(dt);
		}
		if (keys['a']) {
			car.goBackwards(dt);
		}
		if (keys['o']) {
			car.goLeft(dt);
		}
		if (keys['p']) {
			car.goRight(dt);
		}
	}


}

void setCameraTarget() {
	vec3 up(0, 1, 0);
	vec3 cameraPos = cameras[current_camera]->getPosition();
	if (cameraPos.x == 0.0f && cameraPos.y != 0.0f && cameraPos.z == 0.0f) {
		up = vec3(0, 0, 1);
	}
	if (current_camera != 2 && current_camera != 3) {
		cameras[current_camera]->lookAtPoint({ 0, 0, 0 }, up);
	}
}


//--------------------------------------------------------------------------------------------------------//
//                                           FLARE                                                        //
//--------------------------------------------------------------------------------------------------------//
void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport, VSShaderLib* shader) {
	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0


	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel
		if (i == 0) {
			glUniform1i(tex_loc3, 14);
		}
		else {
			glUniform1i(tex_loc3, 3);
		}

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader->getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);
			loc = glGetUniformLocation(shader->getProgramIndex(), "mat.texCount");
			glUniform1i(loc, 4);

			if (i == 0) {
				glActiveTexture(GL_TEXTURE14);
				glBindTexture(GL_TEXTURE_2D, TextureArray[14]);
			}
			else {
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, TextureArray[3]);
			}
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(myMeshes[5].vao);
			glDrawElements(myMeshes[5].type, myMeshes[5].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void drawFlare(VSShaderLib* shader) {
	int flarePos[2];
	int m_viewport[4];
	float light_view_pos[4];

	//Make light position the first candle
	float light_position[4] = { candles.at(0).getPointLight().position.x, candles.at(0).getPointLight().position.y, candles.at(0).getPointLight().position.z, 1.0f };

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

	if (!project(light_position, lightScreenPos, m_viewport))
		printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
	flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
	flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
	popMatrix(MODEL);

	//viewer looking down at  negative z direction
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport, shader);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
}

//--------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------//
//                                           MIRROR                                                       //
//--------------------------------------------------------------------------------------------------------//
void drawMirror(VSShaderLib* shader) {
	MeshBuilder builder;
	pushMatrix(MODEL);
	rotate(MODEL, -90.0f, 0.0f, 1.0f, 0.0f);
	translate(MODEL, 0.0f, 2.0f, 0.0f);
	scale(MODEL, 10.0f, 4.0f, 10.0f);
	glEnable(GL_DEPTH_TEST);
	builder.setShaders(shader, myMeshes[myMeshes.size() - 1]);
	builder.drawMesh(myMeshes[myMeshes.size() - 1], shader);
	popMatrix(MODEL);
}
//--------------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------//
//                                           MIRROR                                                       //
//--------------------------------------------------------------------------------------------------------//
void drawSkybox(VSShaderLib* shader) {
	GLint loc;
	int objId = 3;
	//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CW); // set clockwise vertex order to mean the front

	pushMatrix(MODEL);
	pushMatrix(VIEW);  //se quiser anular a translação

	//  Fica mais realista se não anular a translação da câmara 
	// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
	mMatrix[VIEW][12] = 0.0f;
	mMatrix[VIEW][13] = 0.0f;
	mMatrix[VIEW][14] = 0.0f;

	scale(MODEL, 300.0f, 300.0f, 300.0f);
	translate(MODEL, -0.5f, -0.5f, -0.5f);

	loc = glGetUniformLocation(shader->getProgramIndex(), "mat.texCount");
	glUniform1i(loc, myMeshes[objId].mat.texCount);
	loc = glGetUniformLocation(shader->getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
	loc = glGetUniformLocation(shader->getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
	loc = glGetUniformLocation(shader->getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);

	GLint diffMapCount_loc = glGetUniformLocation(shader->getProgramIndex(), "diffMapCount");
	glUniform1i(diffMapCount_loc, 0);


	// send matrices to OGL
	glUniformMatrix4fv(model_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //Transformação de modelação do cubo unitário para o "Big Cube"
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);

	glBindVertexArray(myMeshes[objId].vao);
	glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	popMatrix(MODEL);
	popMatrix(VIEW);

	glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
	glDepthMask(GL_TRUE);
}
//--------------------------------------------------------------------------------------------------------//

void drawObjects(bool repeated, int scene_offset, int shadows = 0) {
	for (int i = 0; i < oranges.size() && !repeated; i++) {
		if (car.checkCollision(oranges.at(i).getBoundingBox())) {
			car.setPosition({ 1.0f, 0.0f, 1.0f });
			car.setSpeed(0.0f);
			car.setDirectionAngle(0.0f);
			car.resetBoundingBox();
			car.decrementLives();
			lives->text = "Lives: " + to_string(car.getLives());
			car.setPoints(car.getPoints() - 20);
			if (car.getLives() == 0) {
				isGameOver = true;
				keys['s'] = true;
				pauseObjects();
				pause->toRender = false;
				game_over->toRender = true;
			}
		}
	}

	if (car.getPosition().x > 50.0f || car.getPosition().x < -50.0f || car.getPosition().z > 50.0f || car.getPosition().z < -50.0f) {
		car.setPosition({ 1.0f, 0.0f, 1.0f });
		car.setSpeed(0.0f);
		car.setDirectionAngle(0.0f);
		car.resetBoundingBox();
		car.decrementLives();
		lives->text = "Lives: " + to_string(car.getLives());
		car.setPoints(car.getPoints() - 20);
		if (car.getLives() == 0) {
			isGameOver = true;
			keys['s'] = true;
			pauseObjects();
			pause->toRender = false;
			game_over->toRender = true;
		}
	}


	vec3 car_pos = car.getPosition();
	if(!repeated) {
		for (auto const& cheerio : road.getVisible()) {
			if (car.checkCollision(cheerio->getBoundingBox())) {
				car.setPoints(car.getPoints() - 20);
				cheerio->collision_reaction(car_pos, 1.1f, -1.0f);
			}
			cheerio->update(dt);
		}
	}
	

	if (road.carPassedFlag(car_pos) && !isPassingFlag) {
		isPassingFlag = true;
		table.changeScenery();
	}

	if (!road.carPassedFlag(car_pos) && isPassingFlag) {
		isPassingFlag = false;
	}

	if (car.checkCollision(butter.getBoundingBox()) && !repeated) {
		car.setPoints(car.getPoints() - 20);
		butter.collision_reaction(car_pos, 1.1f, -1.0f);
	}
	butter.update(dt);


	for (int i = 0; i < candles.size(); i++) {
		candles.at(i).drawCandle(shader);
	}
	if(!shadows) {
		table.drawTable(shader, TextureArray, 4, scene_offset);
	}
	

	for (int i = 0; i < oranges.size(); i++) {
		if (!repeated && oranges.at(i).getPosition().x < 0) {
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}
		if (repeated && oranges.at(i).getPosition().x > 0 && abs(oranges.at(i).getPosition().z) >= 5.0f) {
			//printf("%f %f %f\n", oranges.at(i).getPosition().x, oranges.at(i).getPosition().y, oranges.at(i).getPosition().z);
			oranges.at(i).drawOrange(shader);
		}
		else if (!repeated) {
			oranges.at(i).drawOrange(shader);
		}

		if (!repeated && oranges.at(i).getPosition().x < 0) {
			glStencilFunc(GL_EQUAL, 0, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}
	}
	points->text = "Points: " + to_string(static_cast<int>(car.getPoints()));
	
	drawFlare(shader);	

	butter.drawButter(shader);
	//car.drawBoundingBox(shader);
}

void setLights(float reflected) {
	GLint loc;
	float res[4];
	float dirlight_direction[4] = { 0.0f, -1.0f, 0.0f, 0.0f };
	multMatrixPoint(VIEW, dirlight_direction, res);
	loc = glGetUniformLocation(shader->getProgramIndex(), "uni_dirlight.direction");
	glUniform4fv(loc, 1, res);
	loc = glGetUniformLocation(shader->getProgramIndex(), "dir_on");
	glUniform1i(loc, directionalLight.on);

	int count = 0;
	for (int i = 0; i < candles.size(); i++) {
		float mult[4];
		vec4 light_pos = candles.at(i).getPointLight().position;
		float values[4] = { light_pos.x * reflected, light_pos.y, light_pos.z, light_pos.w };
		multMatrixPoint(VIEW, values, mult);
		string location = "uni_pointlights[" + to_string(i) + "].position";
		loc = glGetUniformLocation(shader->getProgramIndex(), location.c_str());
		glUniform4fv(loc, 1, mult);

	}
	loc = glGetUniformLocation(shader->getProgramIndex(), "point_on");
	glUniform1i(loc, pointLights_on);
}


void drawShadowPlane(VSShaderLib* shader) {
	MeshBuilder builder;
	struct MyMesh base;
	float amb[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 0.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float shininess = 500.0f;
	int texcount = 0;
	vec3 pos = { 0,0,0 };
	base = createCube();
	base = builder.setMesh(base, amb, diff, spec, emissive, shininess, texcount, pos);

	pushMatrix(MODEL);
	scale(MODEL, 100.0f, 1, 100.0f);
	translate(MODEL, - 0.5f, -1, - 0.5f);
	builder.setShaders(shader, base);
	builder.drawMesh(base, shader);
	popMatrix(MODEL);
}

// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {
	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - oldTime) / 1000;
	oldTime = t;

	float backup_view[16];

	GLint loc;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FrameCount++;
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	// use our shader
	glUseProgram(shader->getProgramIndex());
	setCameraTarget();

	float res[4];
	multMatrixPoint(VIEW, lightPos, res);
	glUniform4fv(lPos_uniformId, 1, res);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);


	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, TextureArray[8]);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[9]);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[10]);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, TextureArray[13]);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, TextureArray[15]);

	glUniform1i(tex_loc0, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);
	glUniform1i(tex_loc8, 8);
	glUniform1i(tex_normalMap_loc, 13);

	

	if (table.getScenery() == 0) {
		glUniform1i(tex_cube_loc, 10);
	}
	else {
		glUniform1i(tex_cube_loc, 9);
	}

	glUniform1i(pause_on_Id, keys['s']);


	car.update(dt);
	for (int i = 0; i < oranges.size(); i++) {
		oranges.at(i).updateSpeed(t);
		oranges.at(i).updatePosition(table_pos, 100.0f, 100.0f, dt);
	}


	pushMatrix(VIEW);
	glDisable(GL_CULL_FACE);
	if (current_camera == 3) {
		car.drawCar(shader, cameras[3], 4, TextureArray, false, cameras[4]);
	}
	else {
		car.drawCar(shader, cameras[2], 4, TextureArray, false, NULL);
	}

	memcpy(backup_view, mMatrix[VIEW], 16 * sizeof(float));
	glEnable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_CULL_FACE);
	popMatrix(VIEW);
	pushMatrix(VIEW);
	pushMatrix(PROJECTION);
	cameras[4]->setViewPort(WinX, WinY);
	if (current_camera == 3) {
		car.drawCar(shader, cameras[3], 3, TextureArray, true, cameras[4]);
		drawObjects(true, 0);
		drawSkybox(shader);
		setLights(1.0f);
		vec3 cam_pos = cameras[current_camera]->getPosition();
		vec3 cam_direction = vec3(car.getPosition().x - cam_pos.x, car.getPosition().y - cam_pos.y, car.getPosition().z - cam_pos.z);
		road.draw(shader, cam_pos, cam_direction.normalize(), current_camera == 2);
	}
	glEnable(GL_CULL_FACE);
	popMatrix(VIEW);
	popMatrix(PROJECTION);


	float car_pos_x = car.getPosition().x;
	if (car_pos_x < 0) {
		pushMatrix(VIEW);
		memcpy(mMatrix[VIEW], backup_view, 16 * sizeof(float));
		glStencilFunc(GL_NEVER, 2, 0x1);
		glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
		drawMirror(shader);
		pushMatrix(MODEL);
		glStencilFunc(GL_EQUAL, 2, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glCullFace(GL_FRONT);
		scale(MODEL, -1.0f, 1.0f, 1.0f);
		setLights(-1.0f);
		pushMatrix(MODEL);
		multMatrix(MODEL, car.getBodyTransformations());
		car.carRecursiveDraw(NULL, NULL, shader, 3, TextureArray);
		popMatrix(MODEL);
		drawObjects(true, 1);
		drawSkybox(shader);
		glCullFace(GL_BACK);
		popMatrix(MODEL);
		popMatrix(VIEW);
	}

	pushMatrix(VIEW);
	memcpy(mMatrix[VIEW], backup_view, 16 * sizeof(float));
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	setLights(1.0f);
	if (table.getScenery() == 0) {
		glUniform1i(glGetUniformLocation(shader->getProgramIndex(), "cubeMap"), 10);
	}
	else {
		glUniform1i(glGetUniformLocation(shader->getProgramIndex(), "cubeMap"), 9);
	}
	drawSkybox(shader);
	drawObjects(false, 0);
	bb.drawBillBoard(cameras[current_camera]->getPosition(), shader, 0);
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	drawShadowPlane(shader);

	float mat[16];
	float plano_chao[4] = { 0, 1, 0, 0 };
	float light_pos[4] = { 0, 20, 0, 1 };


	glUniform1i(shadowMode_uniformId, 1);  //Render with constant color
	shadow_matrix(mat, plano_chao, light_pos);

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	pushMatrix(MODEL);
	multMatrix(MODEL, mat);
	vec3 camera_pos = cameras[current_camera]->getPosition();
	vec3 camera_direction = vec3(car.getPosition().x - camera_pos.x, car.getPosition().y - camera_pos.y, car.getPosition().z - camera_pos.z);
	road.draw(shader, camera_pos, camera_direction.normalize(), current_camera == 2);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	multMatrix(MODEL, mat);
	multMatrix(MODEL, car.getBodyTransformations());
	car.carRecursiveDraw(NULL, NULL, shader, 3, TextureArray);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	multMatrix(MODEL, mat);
	drawObjects(true, 0, 1);
	popMatrix(MODEL);
	glUniform1i(shadowMode_uniformId, 0);


	vec3 cam_pos = cameras[current_camera]->getPosition();
	vec3 cam_direction = vec3(car.getPosition().x - cam_pos.x, car.getPosition().y - cam_pos.y, car.getPosition().z - cam_pos.z);
	road.draw(shader, cam_pos, cam_direction.normalize(), current_camera == 2);

	glEnable(GL_DEPTH_TEST);



	pushMatrix(MODEL);
	multMatrix(MODEL, car.getBodyTransformations());
	glDisable(GL_CULL_FACE);
	car.carRecursiveDraw(NULL, NULL, shader, 3, TextureArray);
	glEnable(GL_CULL_FACE);
	popMatrix(MODEL);

	drawFlare(shader);
	
	

	popMatrix(VIEW);
	glStencilFunc(GL_ALWAYS, 1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	hud.renderText(shaderText);

	update();

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//


void processReleaseKeys(unsigned char key, int xx, int yy) {
	if (key != 's') keys[key] = false;
}

void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;

	case 'c':
		pointLights_on = !pointLights_on;
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'n': directionalLight.on = !directionalLight.on; break;
	case 'h': car.turnOnOffLights(); break;

		// Camera keys
	case '1': current_camera = 0; cameras[current_camera]->setViewPort(WinX, WinY); break;
	case '2': current_camera = 1; cameras[current_camera]->setViewPort(WinX, WinY); break;
	case '3': current_camera = 2; cameras[current_camera]->setViewPort(WinX, WinY); break;
	case '4': current_camera = 3; cameras[current_camera]->setViewPort(WinX, WinY); break;

		// Car movement keys
	case 'q': keys['q'] = true; break;
	case 'o': keys['o'] = true; break;
	case 'a': keys['a'] = true; break;
	case 'p': keys['p'] = true; break;

	case 'e': table.Erupt(); break;

		//Reset game
	case 'r': if (isGameOver) resetGame(); break;

		//Pause
	case 's': if (!isGameOver) { keys['s'] = !keys['s']; pauseObjects(); pause->toRender = keys['s']; } break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN) {
		mouse_pressed = true;
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON) { tracking = 1; leftKey = true; }

		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;

	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		mouse_pressed = false;
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {

		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = 5 * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = 5 * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = 5 * sin(betaAux * 3.14f / 180.0f);
	cameras[2]->setSphericCoords(vec3(camX, camY, camZ));
	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);

	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader->init();
	shader->loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader->loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader->getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader->getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader->getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader->getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
	glBindAttribLocation(shader->getProgramIndex(), TANGENT_ATTRIB, "tangent");


	glLinkProgram(shader->getProgramIndex());
	texMode_uniformId = glGetUniformLocation(shader->getProgramIndex(), "texMode");
	pvm_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader->getProgramIndex(), "l_pos");
	pause_on_Id = glGetUniformLocation(shader->getProgramIndex(), "pause_on");
	tex_loc0 = glGetUniformLocation(shader->getProgramIndex(), "texmap0");
	tex_loc1 = glGetUniformLocation(shader->getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader->getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader->getProgramIndex(), "texmap3");
	tex_loc8 = glGetUniformLocation(shader->getProgramIndex(), "texmap8");
	tex_cube_loc = glGetUniformLocation(shader->getProgramIndex(), "cubeMap");
	model_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_Model");
	shadowMode_uniformId = glGetUniformLocation(shader->getProgramIndex(), "shadowMode");
	tex_normalMap_loc = glGetUniformLocation(shader->getProgramIndex(), "normalMap");
	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader->getAllInfoLogs().c_str());

	shaderText->init();
	shaderText->loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText->loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText->getProgramIndex());

	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shaderText->getAllInfoLogs().c_str());


	return(shader->isProgramLinked() && shaderText->isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

int setMesh(MyMesh mesh, float* amb, float* diff, float* spec, float* emissive, float shininess, int texcount) {
	memcpy(mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh.mat.emissive, emissive, 4 * sizeof(float));
	mesh.mat.shininess = shininess;
	mesh.mat.texCount = texcount;
	myMeshes.push_back(mesh);
	return myMeshes.size() - 1;
}

void init()
{
	srand(time(0));

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;
	int skyboxcount = 9;

	isGameOver = false;

	current_camera = 0;

	table_pos = vec3(0.0f, 0.0f, 0.0f);
	car_pos = vec3(1.0f, 0.0f, 1.0f);
	butter_pos = vec3(10.0f, 0.0f, 10.0f);
	orange_pos = vec3(5.0f, 0.0f, 5.0f);


	table = Table(100.0f, 100.0f, 0.8f, 0.5f, 10.0f, table_pos);
	car = Car(car_pos, 2.5f, 20.0f, car_color, color_tire);
	butter = Butter(butter_pos, butter_foil_color);
	road = Road(vec3(0.0f, 0.0f, 0.0f));
	cheerios.clear();
	oranges.clear();
	candles.clear();

	directionalLight.direction = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	directionalLight.on = 1;

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	lives = new textToRender("Lives: " + to_string(car.getLives()), { WinX / 10.0f, WinY / 10.0f });
	pause = new textToRender("PAUSE", { (WinX / 2.0f) - 75.0f, WinY / 2.0f }, false);
	points = new textToRender("Points: " + to_string(car.getPoints()), { WinX / 10.0f, WinY * 9.0f / 10.0f });
	game_over = new textToRender("GAME OVER", { (WinX / 2.0f) - 100.0f, WinY / 2.0f }, false);

	hud.addText(lives);
	hud.addText(pause);
	hud.addText(points);
	hud.addText(game_over);

	//Texture Object definition

	glGenTextures(16, TextureArray);
	Texture2D_Loader(TextureArray, "vulcan.jpg", 0);
	Texture2D_Loader(TextureArray, "vulcan.jpg", 1);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2);
	Texture2D_Loader(TextureArray, "cactus.png", 8);
	Texture2D_Loader(TextureArray, "particle.tga", 3);

	//Sky Box Texture Object
	const char* mountain_skybox[] = { "right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png" };

	TextureCubeMap_Loader(TextureArray, mountain_skybox, 9);

	const char* volcano_skybox[] = { "right_volc.jpg", "left_volc.jpg", "top_volc.jpg", "bottom_volc.jpg", "front_volc.jpg", "back_volc.jpg" };

	TextureCubeMap_Loader(TextureArray, volcano_skybox, 10);

	Texture2D_Loader(TextureArray, "sand_normal2.jpg", 13);
	Texture2D_Loader(TextureArray, "sun.tga", 14);
	Texture2D_Loader(TextureArray, "sand.jpg", 15);

	MyMesh* torus = new MyMesh;
	float diff1[] = { 1.0f, 0.874f, 0.0f, 1.0f };
	torus = new MyMesh(createTorus(0.2, 0.4, 20, 20));
	memcpy(torus->mat.ambient, amb, 4 * sizeof(float));
	memcpy(torus->mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(torus->mat.specular, spec, 4 * sizeof(float));
	memcpy(torus->mat.emissive, emissive, 4 * sizeof(float));
	torus->mat.shininess = shininess;
	torus->mat.texCount = texcount;

	road.setMesh(torus);
	road.doRoad(25);
	road.doLeftCurve();
	road.doRoad(25);
	road.doLeftCurve();
	road.doRoad(50);
	road.doLeftCurve();
	road.doRoad(25);
	road.doLeftCurve();
	road.doRoad(25);
	road.doFinishLine();

	int n_cherrios = rand() % 5;
	int offset[2] = { -1, 1 };

	Candle candle1(vec3(50, 0, 0), butter_foil_color);
	Candle candle2(vec3(0, 0, 50), butter_foil_color);
	Candle candle3(vec3(-50, 0, 0), butter_foil_color);
	Candle candle4(vec3(0, 0, -50), butter_foil_color);
	Candle candle5(vec3(50, 0, 50), butter_foil_color);
	Candle candle6(vec3(-50, 0, -50), butter_foil_color);

	candle1.createCandle();
	candle2.createCandle();
	candle3.createCandle();
	candle4.createCandle();
	candle5.createCandle();
	candle6.createCandle();

	candles.push_back(candle1);
	candles.push_back(candle2);
	candles.push_back(candle3);
	candles.push_back(candle4);
	candles.push_back(candle5);
	candles.push_back(candle6);

	car.createCar(TextureArray, 4);
	table.createTable(TextureArray, 5);
	butter.createButter();

	Orange orange1({ 1.0, 0.0, 4.0 }, { 0.7f, 0.2f, 0.0f, 1.0f }, color_tire, 1.0f, 5.0f, 0);
	Orange orange2(orange_pos, { 0.7f, 0.2f, 0.0f, 1.0f }, color_tire, 1.0f, 6.0f, 0);
	Orange orange3(orange_pos, { 0.7f, 0.2f, 0.0f, 1.0f }, color_tire, 1.0f, 7.0f, 0);
	Orange orange4(orange_pos, { 0.7f, 0.2f, 0.0f, 1.0f }, color_tire, 1.0f, 6.0f, 0);
	Orange orange5(orange_pos, { 0.7f, 0.2f, 0.0f, 1.0f }, color_tire, 1.0f, 5.0f, 0);

	orange1.createOrange();
	orange2.createOrange();
	orange3.createOrange();
	orange4.createOrange();
	orange5.createOrange();

	oranges.push_back(orange1);
	oranges.push_back(orange2);
	oranges.push_back(orange3);
	oranges.push_back(orange4);
	oranges.push_back(orange5);

	bb.createBillBoard();

	MyMesh amesh;
	float height = 10.0f;
	amesh = createCylinder(height, 0.05f, 10);
	amesh.position = vec3(0, 0, height / 2);
	amesh.rotAngle = 90.0f;
	amesh.rotation = vec3(1.0f, 0.0f, 0.0f);
	float blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	setMesh(amesh, blue, blue, spec, emissive, shininess, texcount);

	amesh = createCylinder(height, 0.05f, 10);
	amesh.position = vec3(0.0f, height / 2, 0.0f);
	amesh.rotAngle = 90.0f;
	amesh.rotation = vec3(0.0f, 1.0f, 0.0f);
	float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	setMesh(amesh, red, red, spec, emissive, shininess, texcount);

	amesh = createCylinder(height, 0.05f, 10);
	amesh.position = vec3(height / 2, 0.0f, 0.0f);
	amesh.rotAngle = 90.0f;
	amesh.rotation = vec3(0.0f, 0.0f, 1.0f);
	float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	setMesh(amesh, green, green, spec, emissive, shininess, texcount);

	amesh = createCube();
	setMesh(amesh, amb, diff, spec, emissive, shininess, skyboxcount);

	helpCube = createCube();
	setMesh(helpCube, red, red, spec, emissive, shininess, texcount);

	amesh = createQuad(1, 1);
	myMeshes.push_back(amesh);
	loadFlareFile(&AVTflare, "flare.txt");

	float amb_mirror[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float diff_mirror[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float spec_mirror[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive_mirror[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	amesh = createQuad(1, 1);
	setMesh(amesh, amb_mirror, diff_mirror, spec_mirror, emissive_mirror, shininess, 0);


	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.55f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //may need to remove this
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);


}

void resetGame() {
	//	Camera initialization	
	OrtographicCamera camera1({ 0.0f, 15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, -1000.0f, 16.0f, -55.0f, 55.0f, 55.0f, -55.0f);
	PerspectiveCamera camera2({ 0.0f, 100.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, 0.01f, -1000.0f, 53.13f);
	PerspectiveCamera camera3({ -4.0f,1.0f,1.0f }, { 0, 0, 0 }, false, 0.1f, 1000.0f, 53.13f);

	currentPosition = vec3(-4, 1, 1);
	camera_angle_xz = M_PI;

	cameras[0] = &camera1;
	cameras[1] = &camera2;
	cameras[2] = &camera3;

	hud.resetHud();
	for (int i = 0; i < 256; i++) {
		keys[i] = false;
	}

	init();

	glutMainLoop();

}



// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char** argv) {


	//	Camera initialization	
	OrtographicCamera camera1({ 0.0f, 15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, -1000.0f, 16.0f, -55.0f, 55.0f, 55.0f, -55.0f);
	PerspectiveCamera camera2({ 0.0f, 100.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, 0.01f, -1000.0f, 53.13f);
	PerspectiveCamera camera3({ -4.0f,1.0f,1.0f }, { 0, 0, 0 }, false, 0.1f, 1000.0f, 53.13f);
	PerspectiveCamera camera4({ -0.2f, 0.0f, 0.0f }, { 0, 0, 0 }, false, 0.1f, 1000.0f, 53.13f);
	PerspectiveCamera camera5({ 2.0f, 0.6f, 0.0f }, { 0, 0, 0 }, false, 0.1f, 1000.0f, 53.13f);
	camera4.setSphericCoords(vec3(-0.4f, 0.5f, 0.001f));
	camera5.setSphericCoords(vec3(2.0f, 0.6f, 0.001f));

	currentPosition = vec3(-4, 1, 1);

	cameras[0] = &camera1;
	cameras[1] = &camera2;
	cameras[2] = &camera3;
	cameras[3] = &camera4;
	cameras[4] = &camera5;

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


	//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(1000 / 60, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processReleaseKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc(mouseWheel);



	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}
