/**************************************************************************

  File: gameWorld.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/

#include <iostream>
#include <GL/glut.h>
//#include <IL/config.h>
#include <IL/ilut.h>
#include <cmath>
#include "gameWorld.h"

//initial starting eye location
float x = 0.0f;
float y = 10.0f;
float z = 0.0f;
//initial center of projection
float lx = 0.0f;
float ly = 0.0f;
float lz = -1.0f;
float walkStraightSpeed = 0.2;
float walkSideWaySpeed = 0.2;
float currStraightSpeed = walkStraightSpeed;
float currSidewaySpeed = walkSideWaySpeed;
//orientations
float angleHorizontal = 0.0;
float angleVertical = 0.0;
float dAngleHorizontal = 0.0;
float dAngleVertical = 0.0;
float dMoveStraight = 0.0;
float dMoveSideways = 0.0;

//Global lighting
const GLfloat light_ambient[]  = { 0.2f, 0.2f, 0.2f, 0.2f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position0[] = { 1.0f, 1.0f, 1.0f, 0.0f };

//Material lighting
const GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 0.2f };
const GLfloat mat_diffuse[] = { 0.0f, 0.0f, 0.0f, 0.0f };
const GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 0.1f };
const GLfloat high_shininess[] = { 128.0f };

int timeLastLoop = 0.0;

GLuint stoneTexName = 0;
GLuint crateTexName = 0;
GLuint boxDL = 0;

//temporary collision shapes
btCollisionShape* sphereShape;
btCollisionShape* boxShape;

//spheres
struct Spheres
{
	btDefaultMotionState* sphereMotionState;
	btRigidBody* sphereRigidBody;
};

const int numSpheres = 25;
Spheres sphereList[numSpheres];

//box
struct Boxes
{
	btDefaultMotionState* boxMotionState;
	btRigidBody* boxRigidBody;
};

const int numBoxes = 25;
Boxes boxList[numBoxes];

/******************************************************************
  Function:   constructor and destructor
  Purpose:
******************************************************************/
GameWorld::GameWorld()
{
   char datfilename[] = "track1.dat";
	debugMode = DEBUG_MODE_1;
	cameraMode = CAMERA_CAR;
	m_cameraPosition = btVector3(30,30,30);
	m_cameraHeight = 4.f;
	m_minCameraDistance = 3.f;
	m_maxCameraDistance = 10.f;
	initWorld();
	raceMapDL = -1;

	// create a new physics object which initializes the engine
	worldPhysics = new Physics(-20.8);

	// create a new race track and register with physics engine
	raceMap = new RaceTrack(0.0f,50.0f);
	raceMap -> loadMap(datfilename);
	worldPhysics -> registerMap(raceMap);

	// create a new vehicle and register with physics engine
	myVehicle = new Vehicle();
	myVehicle -> setStartingLocation(raceMap -> getStartingLocation());
	myVehicle -> setStartingOrientation(raceMap -> getStartingOrientation());
	myVehicle -> initVehicle("default");
	worldPhysics -> registerVehicle(myVehicle);

	// set debug object
	gDebugDrawer = new GLDebugDrawer();
	worldPhysics -> getDynamicsWorld() -> setDebugDrawer(gDebugDrawer);

	tempInitSimulate();
	tempLoadTextures();
	createBoxDL();

	ILenum Error;
   while ((Error = ilGetError()) != IL_NO_ERROR) {
       std::cout << Error << ": " << iluErrorString(Error) << std::endl;
   }
}

GameWorld::~GameWorld()
{
	deleteTempObjects();
	if (worldPhysics) delete worldPhysics;
	if (raceMap) delete raceMap;
	if (myVehicle) delete myVehicle;
	if (gDebugDrawer) delete gDebugDrawer;

	worldPhysics = NULL;
	raceMap = NULL;
	myVehicle = NULL;
}

/******************************************************************
  Function:   initWorld
  Purpose:
******************************************************************/
void GameWorld::initWorld()
{
	// Enable texture to be used
	glEnable(GL_TEXTURE_2D);
	// Set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_2D);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);  //sets lighting to one-sided
	glEnable(GL_COLOR_MATERIAL);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glColorMaterial(GL_FRONT,GL_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

/******************************************************************
  Function:   updateCamera
  Purpose:    update where the camera looks
******************************************************************/
void GameWorld::updateCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	btTransform chassisWorldTrans;

	//look at the vehicle
	myVehicle->getMotionState()->getWorldTransform(chassisWorldTrans);
	m_cameraTargetPosition = chassisWorldTrans.getOrigin();

	//interpolate the camera height
	m_cameraPosition[1] = (15.0*m_cameraPosition[1] + m_cameraTargetPosition[1] + m_cameraHeight)/16.0;

	btVector3 camToObject = m_cameraTargetPosition - m_cameraPosition;

	//keep distance between min and max distance
	float cameraDistance = camToObject.length();
	float correctionFactor = 0.f;
	if (cameraDistance < m_minCameraDistance)
	{
		correctionFactor = 0.15*(m_minCameraDistance-cameraDistance)/cameraDistance;
	}
	if (cameraDistance > m_maxCameraDistance)
	{
		correctionFactor = 0.15*(m_maxCameraDistance-cameraDistance)/cameraDistance;
	}
	m_cameraPosition -= correctionFactor*camToObject;

	//update OpenGL camera settings
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(m_cameraPosition[0],m_cameraPosition[1]+2.0,m_cameraPosition[2],
		      m_cameraTargetPosition[0],m_cameraTargetPosition[1], m_cameraTargetPosition[2],
			  0,1,0);
}

//******************************************************************
//  Function:   updateCameraLocation
//  Purpose:
//******************************************************************
void GameWorld::updateCameraLocation()
{
	if (dMoveStraight != 0.0)
	{
		x += (dMoveStraight*lx);
		z += (dMoveStraight*lz);
		y += (dMoveStraight*ly);
	}
	if (dMoveSideways)
	{
		x -= (dMoveSideways*lz);
		z += (dMoveSideways*lx);
	}
}

//******************************************************************
//  Function:   updateCameraOrientation
//  Purpose:
//******************************************************************
void GameWorld::updateCameraOrientation()
{
	if (dAngleHorizontal)
	{
		angleHorizontal += dAngleHorizontal;
		lx = sin(angleHorizontal);
		lz = -cos(angleHorizontal);
	}
	if (dAngleVertical)
	{
		angleVertical += (dAngleVertical * 0.1);
		if (angleVertical > 1.57) angleVertical = 1.57;
		else if (angleVertical < -1.57) angleVertical = -1.57;
		ly = -sin(angleVertical);
	}
}

/******************************************************************
  Function:   renderWorld
  Purpose:    Function to render the world and its objects
******************************************************************/
void GameWorld::renderWorld()
{
	if (cameraMode == CAMERA_FREE)
	{
		updateCameraLocation();
		updateCameraOrientation();
		glLoadIdentity();
		gluLookAt(x, y, z,
			x + 9*lx,y + 45*ly,z + 9*lz,
			0.0f,1.0f,0.0f);
	}
	else updateCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	raceMap->renderRaceTrack();

	btTransform trans;
	drawTempObjects();
	myVehicle->renderVehicle();
	worldPhysics->getDynamicsWorld()->debugDrawWorld();
	glutSwapBuffers();
}

/******************************************************************
  Function:   updateWorld
  Purpose:    this function is called whenever the reshape
              callback function is executed during game play
******************************************************************/
void GameWorld::updateWorld()
{
	myVehicle -> updateVehicle();

	int timeNow = glutGet(GLUT_ELAPSED_TIME);
	float timeStep = (float)(timeNow - timeLastLoop)/1000.0;
	timeLastLoop = timeNow;
	worldPhysics -> stepSimulation(timeStep);

	glutPostRedisplay();
}

/******************************************************************
  Function:   updateCameraOrientation
  Purpose:    reshape function that is used when screen is
              resized during game play
******************************************************************/
void GameWorld::reshapeWorld(int ww, int wh)
{
	float ratio = 1.0f * ww / wh;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, ww, wh);
	// Set the clipping volume
	gluPerspective(45,ratio,0.1,1000);
	// setting the camera now
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx,y + ly,z + lz,
		0.0f, 1.0f, 0.0f);

	//clear and flush
	glClearColor (0.85, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
}

/******************************************************************
  Function:   nextDebugMode
  Purpose:    whenever called it will set the next debug mode
              available. First debug mode is off.
******************************************************************/
void GameWorld::nextDebugMode()
{
	if (debugMode == DEBUG_MODE_1)
	{
		debugMode = DEBUG_MODE_2;
		worldPhysics -> getDynamicsWorld() -> getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawAabb);
	}
	else if (debugMode == DEBUG_MODE_2)
	{
		debugMode = DEBUG_MODE_3;
		worldPhysics -> getDynamicsWorld() -> getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	}
	else if (debugMode == DEBUG_MODE_3)
	{
		debugMode = DEBUG_MODE_4;
		worldPhysics -> getDynamicsWorld() -> getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	}
	else
	{
		debugMode = DEBUG_MODE_1;
		worldPhysics -> getDynamicsWorld() -> getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
	}
}

/******************************************************************
  INPUT FUNCTIONS. Called from main application
  function names should be self explanatory
******************************************************************/

void GameWorld::keySpecialDownEvent(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			if (cameraMode == CAMERA_FREE) dAngleVertical = -0.05f;
			else myVehicle -> applyAccelerationForce(true);
			break;
		case GLUT_KEY_DOWN:
			if (cameraMode == CAMERA_FREE) dAngleVertical = 0.05f;
			else myVehicle -> applyBrakingForce(true);
			break;
		case GLUT_KEY_LEFT:
			if (cameraMode == CAMERA_FREE) dAngleHorizontal = -0.025f;
			else myVehicle -> applySteeringLeft(true);
			break;
		case GLUT_KEY_RIGHT:
			if (cameraMode == CAMERA_FREE) dAngleHorizontal = 0.025f;
			else myVehicle -> applySteeringRight(true);
			break;
	}
}

void GameWorld::keySpecialUpEvent(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			if (cameraMode == CAMERA_FREE && dAngleVertical < 0.0f) dAngleVertical = 0.0f;
			else myVehicle -> applyAccelerationForce(false);
			break;
		case GLUT_KEY_DOWN:
			if (cameraMode == CAMERA_FREE && dAngleVertical > 0.0f) dAngleVertical = 0.0f;
			else myVehicle -> applyBrakingForce(false);
			break;
		case GLUT_KEY_LEFT:
			if (cameraMode == CAMERA_FREE && dAngleHorizontal < 0.0f) dAngleHorizontal = 0.0f;
			else myVehicle -> applySteeringLeft(false);
			break;
		case GLUT_KEY_RIGHT:
			if (cameraMode == CAMERA_FREE && dAngleHorizontal > 0.0f) dAngleHorizontal = 0.0f;
			else myVehicle -> applySteeringRight(false);
			break;

		case GLUT_KEY_F1:
			cameraMode = CAMERA_CAR;
			break;
		case GLUT_KEY_F2:
			cameraMode = CAMERA_FREE;
			break;
	}
}

void GameWorld::keyRegularDownEvent(unsigned char key, int x, int y)
{
	switch (key) {
		case 'w': case 'W':
			if (cameraMode == CAMERA_FREE) dMoveStraight = currStraightSpeed;
			break;
		case 's': case 'S':
			if (cameraMode == CAMERA_FREE) dMoveStraight = -currStraightSpeed;
			break;
		case 'd': case 'D':
			if (cameraMode == CAMERA_FREE) dMoveSideways = currSidewaySpeed;
			break;
		case 'a': case 'A':
			if (cameraMode == CAMERA_FREE) dMoveSideways = -currSidewaySpeed;
			break;

		case 'z': case 'Z':
			myVehicle -> toggleReverse();
			break;
		case 'g': case 'G':
			nextDebugMode();
			break;

		case '1':
			myVehicle -> changeVehicleModel("default");
			break;
		case '2':
			myVehicle -> changeVehicleModel("car1");
			break;
		case '3':
			myVehicle -> changeVehicleModel("jeep1");
			break;
		case '4':
			myVehicle -> changeVehicleModel("jeep2");
			break;
		case '5':
			myVehicle -> changeVehicleModel("bus");
			break;
		case '6':
			myVehicle -> changeVehicleModel("4x4");
			break;

		case 'q': case 'Q':
			RacerGameApp::closeApplicationSuccess();
			break;
	}
}

void GameWorld::keyRegularUpEvent(unsigned char key, int x, int y)
{
	switch (key) {
		case 'w': case 'W':
			if (cameraMode == CAMERA_FREE) dMoveStraight = 0.0;
			break;
		case 's': case 'S':
			if (cameraMode == CAMERA_FREE) dMoveStraight = 0.0;
			break;
		case 'd': case 'D':
			if (cameraMode == CAMERA_FREE) dMoveSideways = 0.0;
			break;
		case 'a': case 'A':
			if (cameraMode == CAMERA_FREE) dMoveSideways = 0.0;
			break;
	}
}

/******************************************************************
  TEMPORARY FUNCTIONS
  The following functions were used for the purpose of the
  in class demonstration
******************************************************************/
// initialize simulation of these objects
void GameWorld::tempInitSimulate()
{
	sphereShape = new btSphereShape(4);
	boxShape = new btBoxShape(btVector3(3,3,3));

	btScalar mass = 500.0;
	btVector3 fallInertia(0,0,0);

	// Load spheres into the physics engine
	sphereShape->calculateLocalInertia(mass,fallInertia);
	for (int i = 0; i < numSpheres; ++i)
	{
		sphereList[i].sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(rand()%400 - 200,rand()%30 + 5,rand()%20-335)));
		btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass,sphereList[i].sphereMotionState,sphereShape,fallInertia);
		sphereList[i].sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
		worldPhysics -> getDynamicsWorld() -> addRigidBody(sphereList[i].sphereRigidBody);
	}

	// Load boxes into the physics engine
	boxShape->calculateLocalInertia(mass,fallInertia);
	for (int i = 0; i < numBoxes; ++i)
	{
		boxList[i].boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(rand()%400 - 200,rand()%30 + 5,rand()%20-335)));
		btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI2(mass,boxList[i].boxMotionState,boxShape,fallInertia);
		boxList[i].boxRigidBody = new btRigidBody(boxRigidBodyCI2);
		worldPhysics -> getDynamicsWorld() -> addRigidBody(boxList[i].boxRigidBody);
	}
}

// clean up after ourselves
void GameWorld::deleteTempObjects()
{
	for (int i = 0; i < numSpheres; ++i)
	{
		delete sphereList[i].sphereRigidBody;
		delete sphereList[i].sphereMotionState;
	}

	for (int i = 0; i < numBoxes; ++i)
	{
		delete boxList[i].boxRigidBody;
		delete boxList[i].boxMotionState;
	}
}

// draw spheres
void GameWorld::drawTempObjects()
{
	GLUquadricObj *quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, (GLenum)GLU_FILL);
	gluQuadricNormals(quadObj, (GLenum)GLU_SMOOTH);

	btTransform trans;
	float m[16];

	for (int i = 0; i < numSpheres; ++i)
	{
		sphereList[i].sphereRigidBody->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(m);

		glEnable(GL_COLOR_MATERIAL);
		glPushMatrix();
			glMultMatrixf(m);

			glEnable(GL_TEXTURE_2D);
			gluQuadricTexture(quadObj, true);
			glBindTexture(GL_TEXTURE_2D, stoneTexName);
			gluSphere(quadObj,4.0,16,16);
			gluQuadricTexture(quadObj, false);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	for (int i = 0; i < numBoxes; ++i)
	{
		boxList[i].boxRigidBody->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(m);
		glPushMatrix();
			glMultMatrixf(m);
			glScalef(3.0,3.0,3.0);
			glCallList(boxDL);
		glPopMatrix();
	}

	gluDeleteQuadric(quadObj);
}

// generate display list for cubes with texture
void GameWorld::createBoxDL()
{
	boxDL = glGenLists(1);
	glNewList(boxDL, GL_COMPILE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, crateTexName);  /* select our texture */
		glBegin(GL_QUADS);
			/* front face */
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			/* back face */
			glNormal3f(0.0f, 0.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			/* right face */
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			/* left face */
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			/* top face */
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			/* bottom face */
			glNormal3f(0.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	glEndList();
}

// Load temporary textures
void GameWorld::tempLoadTextures()
{
	// use DevILs ability to load an image and generate OpenGL textures
	// by making the OpenGL calls for us. the texture id is returned
	char stonepath[] = "data/textures/stone.jpg";
	char cratepath[] = "data/textures/crate.bmp";

	stoneTexName = ilutGLLoadImage(stonepath);
	crateTexName = ilutGLLoadImage(cratepath);
}
