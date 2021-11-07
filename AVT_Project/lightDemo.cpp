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

VSShaderLib *shader = new VSShaderLib();
VSShaderLib *shaderText = new VSShaderLib();

//File with the font
const string font_name = "fonts/arial.ttf";

HUD hud;

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
GLint dir_light_uniformId;
GLint pause_on_Id;
GLint tex_loc0, tex_loc1, tex_loc2, tex_loc3;
GLuint TextureArray[10];


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

//----------------Lights---------------------
Light directionalLight;
//-------------------------------------------

bool mouse_pressed = false;
vec3 oldPosition = vec3(0.0f,0.0f,0.0f);
vec3 currentPosition;

void resetGame();

//---------------Particles-------------------
typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posição
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // aceleração
} Particle;
//-------------------------------------------

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
	glutTimerFunc(1000/60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	cameras[current_camera]->setViewPort(w, h);
	WinX = w;
	WinY = h;
	lives->coordinates = { WinX/10.0f, WinY/10.0f };
	pause->coordinates = { (WinX / 2.0f) - 75.0f, WinY / 2.0f };
	points->coordinates = { WinX / 10.0f, WinY * 9.0f / 10.0f };
	game_over->coordinates = { (WinX / 2.0f) - 100.0f, WinY / 2.0f };
}

void pauseObjects() {
	car.setPause(keys['s']);
	butter.setPause(keys['s']);
	for (Orange &o : oranges) {
		o.setPause(keys['s']);
	}

	for (Cheerio *c : road.getLimits()) {
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

void drawObjects(bool repeated) {
	car.update(dt);
	for (int i = 0; i < oranges.size() && !repeated; i++) {
		if (car.checkCollision(oranges.at(i).getBoundingBox())) {
			car.setPosition({1.0f, 0.0f, 1.0f});
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
	
	vec3 car_pos = car.getPosition();
	for (auto const& cheerio : road.getVisible()) {
		if (car.checkCollision(cheerio->getBoundingBox()) && !repeated) {
			car.setPoints(car.getPoints() - 20);
			cheerio->collision_reaction(car_pos, 1.1f, -1.0f);
		}
		cheerio->update(dt);
	}
	
	if (car.checkCollision(butter.getBoundingBox()) && !repeated) {
		car.setPoints(car.getPoints() - 20);
		butter.collision_reaction(car_pos, 1.1f, -1.0f);
	}
	butter.update(dt);
	
	vec3 camera_pos = cameras[2]->getPosition();
	vec3 camera_direction = vec3(car.getPosition().x - camera_pos.x, car.getPosition().y - camera_pos.y, car.getPosition().z - camera_pos.z);
	road.draw(shader, camera_pos, camera_direction.normalize(), current_camera == 2);

	for (int i = 0; i < candles.size(); i++) {
		candles.at(i).drawCandle(shader);
	}
	
	table.drawTable(shader, TextureArray, 4);
	
	for (int i = 0; i < oranges.size(); i++) {
		oranges.at(i).updateSpeed(t);
		oranges.at(i).drawOrange(shader);
		oranges.at(i).updatePosition(table_pos, 100.0f, 100.0f, dt);
	}

	points->text = "Points: " + to_string(static_cast<int>(car.getPoints()));
	//butter.drawButter(shader);
	//car.drawBoundingBox(shader);
	
}

void setLights() {
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
		float values[4] = { light_pos.x, light_pos.y, light_pos.z, light_pos.w };
		multMatrixPoint(VIEW, values, mult);
		string location = "uni_pointlights[" + to_string(i) + "].position";
		loc = glGetUniformLocation(shader->getProgramIndex(), location.c_str());
		glUniform4fv(loc, 1, mult);

	}
	loc = glGetUniformLocation(shader->getProgramIndex(), "point_on");
	glUniform1i(loc, pointLights_on);
}

// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {
	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - oldTime) / 1000;
	oldTime = t;

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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);

	glUniform1i(tex_loc0, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);

	glUniform1i(pause_on_Id, keys['s']);

	int objId = 0;
	pushMatrix(VIEW);
	glDisable(GL_CULL_FACE);
	if (current_camera == 3) {
		car.drawCar(shader, cameras[3], 3, TextureArray, false, cameras[4]);
	}
	else {
		car.drawCar(shader, cameras[2], 3, TextureArray, false, NULL);
	}
	glEnable(GL_CULL_FACE);



	glStencilFunc(GL_NOTEQUAL, 1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	drawObjects(false);

	setLights();

	glStencilFunc(GL_EQUAL, 1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_CULL_FACE);
	popMatrix(VIEW);
	pushMatrix(VIEW);
	pushMatrix(PROJECTION);
	cameras[4]->setViewPort(WinX, WinY);
	if (current_camera == 3) {
		car.drawCar(shader, cameras[3], 3, TextureArray, true, cameras[4]);
		drawObjects(true);
		setLights();
	}
	glEnable(GL_CULL_FACE);
	popMatrix(VIEW);
	popMatrix(PROJECTION);
	glStencilFunc(GL_ALWAYS, 1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDisable(GL_DEPTH_TEST);
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
	if(key != 's') keys[key] = false;
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

	camX = 5*sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = 5*cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = 5*sin(betaAux * 3.14f / 180.0f);
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

	glLinkProgram(shader->getProgramIndex());

	pvm_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader->getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader->getProgramIndex(), "l_pos");
	pause_on_Id = glGetUniformLocation(shader->getProgramIndex(), "pause_on");
	tex_loc0 = glGetUniformLocation(shader->getProgramIndex(), "texmap0");
	tex_loc1 = glGetUniformLocation(shader->getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader->getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader->getProgramIndex(), "texmap3");
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

	isGameOver = false;

	current_camera = 0;

	table_pos = vec3(0.0f, 0.0f, 0.0f);
	car_pos = vec3(1.0f, 0.0f, 1.0f);
	butter_pos = vec3(5.0f, 0.0f, 0.0f);
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

	glGenTextures(10, TextureArray);
	Texture2D_Loader(TextureArray, "stone.tga", 0);
	Texture2D_Loader(TextureArray, "orange.jpg", 1);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2);

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
	road.doRoad(5);
	road.doRightCurve();
	/*road.doNorthRoad(25);
	road.doEastCurve();
	road.doEastRoad(20);
	road.doSouthCurve();
	road.doSouthRoad(50);
	road.doWestSouthCurve();
	road.doWestRoad(50);
	road.doNorthWestCurve();
	road.doNorthRoad(60);
	road.doEastCurve();
	road.doEastRoad(60);
	road.doSouthCurve();
	road.doSouthRoad(70);
	road.doWestSouthCurve();
	road.doWestRoad(70);*/
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

	car.createCar(TextureArray, 3);
	table.createTable(TextureArray, 4);
	butter.createButter();

	Orange orange1(orange_pos, { 0.7f, 0.2f, 0.0f, 0.2f }, color_tire, 1.0f, 5.0f, 0);
	Orange orange2(orange_pos, { 0.7f, 0.2f, 0.0f, 0.2f }, color_tire, 1.0f, 6.0f, 0);
	Orange orange3(orange_pos, { 0.7f, 0.2f, 0.0f, 0.2f }, color_tire, 1.0f, 7.0f, 0);
	Orange orange4(orange_pos, { 0.7f, 0.2f, 0.0f, 0.2f }, color_tire, 1.0f, 6.0f, 0);
	Orange orange5(orange_pos, { 0.7f, 0.2f, 0.0f, 0.2f }, color_tire, 1.0f, 5.0f, 0);

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

	helpCube = createCube();
	setMesh(helpCube, red, red, spec, emissive, shininess, texcount);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.66f, 0.66f, 0.66f, 1.0f);
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
	OrtographicCamera camera1({ 0.0f, 15.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, true, -1000.0f, 16.0f, -55.0f, 55.0f, 55.0f, -55.0f);
	PerspectiveCamera camera2({ 0.0f, 100.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, 0.01f, -1000.0f, 53.13f);
	PerspectiveCamera camera3({-4.0f,1.0f,1.0f}, { 0, 0, 0}, false, 0.1f, 1000.0f, 53.13f);
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
