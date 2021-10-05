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

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <random>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "Table.h"
#include "Car.h"
#include "Orange.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "Butter.h"
#include "Road.h"

using namespace std;

#define CAPTION "AVT Per Fragment Phong Lightning Demo"
int WindowHandle = 0;
int WinX = 640, WinY = 480;

unsigned int FrameCount = 0;

bool keys[256] = { false };

vec3 table_pos(0.0f, 0.0f, 0.0f);

vec3 car_pos(1.0f, 0.0f, 1.0f);
vec3 butter_pos(5.0f, 1.0f, 0.0f);
vec4 car_color(1.0f, 1.0f, 1.0f, 0.7f);
vec4 color_tire(0.1f, 0.1f, 0.1f, 1.0f);
vec4 cheerio_color(1.0f, 0.874f, 0.0f, 1.0f);
vec4 butter_foil_color(0.0f, 0.0f, 0.9f, 1.0f);

vec3 orange_pos(5.0f, 0.0f, 5.0f);

Table table(100.0f, 100.0f, 0.8f, 0.5f, 10.0f, table_pos);
Car car(car_pos, 2.5f, 20.0f, car_color, color_tire);
Butter butter(butter_pos, butter_foil_color);
Orange orange(orange_pos, car_color, color_tire, 1.0f, 15.0f, 0);
Road road(vec3(0.0f, 0.0f, 0.0f));
std::vector<Cheerio> cheerios;


int current_camera = 2;
Camera* cameras[3];

VSShaderLib shader;

//Vector with meshes
vector<struct MyMesh> myMeshes;

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
	
// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 10.0f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

float oldTime = 0.0f;
float dt = 0.0f;

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
	for (int i = 0; i < 2; i++) {
		cameras[i]->setViewPort(w, h);
	}
}

void update() {
	if(!(keys['q'] || keys['a'])) {
		car.stop(dt);
	}
	if(keys['q']) {
		car.goForward(dt);
	} 
	if(keys['a']) {
		car.goBackwards(dt);
	}
	if (keys['o']) {
		car.goLeft(dt);
	}
	if (keys['p']) {
		car.goRight(dt);
	}
	
}

// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	int t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - oldTime) / 1000;
	oldTime = t;

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	vec3 up(0, 1, 0);
	vec3 cameraPos = cameras[current_camera]->getPosition();
	if (cameraPos.x == 0.0f && cameraPos.y != 0.0f && cameraPos.z == 0.0f) {
		up = vec3(0, 0, 1);
	}
	float angle = car.getRotAngle() * M_PI / 180;
	vec3 normalized_speed = vec3(cos(angle), 0, sin(-angle)).normalize() * 5.0f;
	normalized_speed.y = -5;
	cameras[2]->setPosition(car.getPosition() - normalized_speed);
	if (current_camera == 2) {
		cameras[2]->lookAtPoint({ car.getPosition().x, car.getPosition().y, car.getPosition().z }, up);
	}
	else {
		cameras[0]->lookAtPoint({ 0, 0, 0 }, up);
	}
	// use our shader
	glUseProgram(shader.getProgramIndex());

	//send the light position in eye coordinates

		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId, 1, res);

	int objId=0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	table.drawTable(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	car.drawCar(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	butter.drawButter(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	/*for (int i = 0; i < cheerios.size(); i++) {
		cheerios[i].drawCheerio(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	}*/
	orange.drawOrange(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	orange.updatePosition(table_pos, 100.0f, 100.0f, dt);

	road.draw(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);

	for (int i = 0; i < 3; ++i) {

		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[objId].mat.shininess);
		pushMatrix(MODEL);
		translate(MODEL, myMeshes[objId].position.x,  myMeshes[objId].position.y,  myMeshes[objId].position.z);
		rotate(MODEL, myMeshes[objId].rotAngle, myMeshes[objId].rotation.x, myMeshes[objId].rotation.y, myMeshes[objId].rotation.z);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(myMeshes[objId].vao);
			
		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);	
		}
		glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		objId++;
	}
	update();
	car.update(dt);

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//


void processReleaseKeys(unsigned char key, int xx, int yy) {
	keys[key] = false;
}

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:
			glutLeaveMainLoop();
			break;

		case 'c': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
			break;
		case 'm': glEnable(GL_MULTISAMPLE); break;
		case 'n': glDisable(GL_MULTISAMPLE); break;

		// Camera keys
		case '1': current_camera = 0; break;
		case '2': current_camera = 1; break;
		case '3': current_camera = 2; break;
		
		// Car movement keys
		case 'q': keys['q'] = true; break;
		case 'a': keys['a'] = true; break;
		case 'o': keys['o'] = true; break;
		case 'p': keys['p'] = true; break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
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

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

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

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);

	cameras[current_camera]->setPosition({ camX, camY, camZ });
//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	
	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
	
	return(shader.isProgramLinked());
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

	float amb[]= {0.2f, 0.15f, 0.1f, 1.0f};
	float diff[] = {0.8f, 0.6f, 0.4f, 1.0f};
	float spec[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float emissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float shininess= 100.0f;
	int texcount = 0;

	road.doNorthRoad(20);

	int n_cherrios = rand() % 5;
	int offset[2] = { -1, 1 };
	for (int i = 0; i < n_cherrios; i++) {
		int j = rand() % 2;
		vec3 position = vec3(offset[j] * rand() % 45, 0.0f, offset[j] * (rand() % 45));
		Cheerio cheerio(position, vec4(1.0f, 0.8745f, 0.0f, 1.0f));
		cheerio.createCheerio();
		cheerios.push_back(cheerio);
	}
	
	table.createTable();
	car.createCar();
	orange.createOrange();
	butter.createButter();
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

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}


// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {


	//	Camera initialization	
	OrtographicCamera camera1({ 15.0f, 15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, false, 0.1f, 1000.0f, 100.0f, 100.0f, 100.0f, 100.0f);
	PerspectiveCamera camera2({ 0.0f, 15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, 0.1f, 1000.0f, 53.13f);
	PerspectiveCamera camera3({ orange.getPosition().x - 5.0f, orange.getPosition().y + 5.0f, orange.getPosition().z }, { orange.getPosition().x, orange.getPosition().y, orange.getPosition().z }, false, 0.1f, 1000.0f, 53.13f);

	cameras[0] = &camera1;
	cameras[1] = &camera2;
	cameras[2] = &camera3;

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(1000/60, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processReleaseKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	


//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

