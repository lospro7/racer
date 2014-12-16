/**************************************************************************

  File: vehicles.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza
  carlospro7@gmail.com

  Implementation of a vehicle in game.
  Reads a .dat file that contains properties used to render the 3ds
  model properly.
  Then loads the 3ds model specified in the .dat file, and its
  corresponding textures.

  Also prepares physical properties that are used by the Bullet
  Physics engine to simulate a vehicle.

**************************************************************************/
#include <btBulletDynamicsCommon.h>
#include <fstream>
#include <string>
#include <GL/glut.h>
#include <IL/ilut.h>
#include "vehicle.h"

btCollisionShape* Vehicle::vehicleColShape;

/******************************************************************
  Function:   constructor
  Purpose:    initialize variables used by vehicle object
******************************************************************/
Vehicle::Vehicle()
{
	car3DModel = NULL;

	startPoint[0] = startPoint[1] = startPoint[2] = 0.0;
	startOrient[0] = startOrient[1] = startOrient[2] = startOrient[3] = 0.0;
	modelTrans[0] = modelTrans[1] = modelTrans[2] = 0.0;
	modelRotate[0] = modelRotate[1] = modelRotate[2] = modelRotate[3] = 0.0;
	vehicleDL = 0;

	numTextures = 0;
	m_carChassis = NULL;
	m_vehicle = NULL;

	gEngineForce = 0.f;
	gBreakingForce = 100.f;
	maxEngineForce = 4000.f;
	maxBreakingForce = 1000.f;
	wheelRadius = 0.7f;
	wheelWidth = 0.5f;
	suspensionRestLength = btScalar(1);

	suspensionStiffness = 200.f;
	suspensionDamping = 200.f;
	suspensionCompression = 200.4f;
	wheelFriction = 10000;
	rollInfluence = 0.04f;

	gVehicleSteering = 0.f;
	steeringIncrement = 0.001f;
	steeringClamp = 0.2f;

	steerRight = steerLeft = isReverse = false;
}

/******************************************************************
  Function:   initVehicle
  Purpose:    prepare vehicle for usage and prepare to register
	           with physics engine
******************************************************************/
void Vehicle::initVehicle(const char *newVehicleName)
{
	modelName = std::string(newVehicleName);
	loadVehicle(modelName.c_str());

	// the chassis collision shape
	btCollisionShape* chassisShape = new btBoxShape(btVector3(carDims[0],carDims[1],carDims[2]));
	m_collisionShapes.push_back(chassisShape);

	btCompoundShape* compound = new btCompoundShape();
	m_collisionShapes.push_back(compound);
	btTransform localTrans;
	localTrans.setIdentity();

	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(0.0,2.0,0));

	compound->addChildShape(localTrans,chassisShape);

	btTransform transformMe;
	btQuaternion rotateMe;
	btScalar angle = (startOrient[0] * 3.14159265) / 180.0f;

	transformMe.setIdentity();
	// set initial location of vehicle in the world
	transformMe.setOrigin(btVector3(startPoint[0],startPoint[1],startPoint[2]));
	rotateMe.setRotation(btVector3(startOrient[1],startOrient[2],startOrient[3]),angle);
	transformMe.setRotation(rotateMe);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	float mass = 1000.0f;
	btVector3 localInertia(0,0,0);

	compound->calculateLocalInertia(mass,localInertia);

	vehicleMotionState = new btDefaultMotionState(transformMe);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,vehicleMotionState,compound,localInertia);
	m_carChassis = new btRigidBody(cInfo);
	//m_carChassis->setDamping(0.2,0.2);

	m_carChassis -> setLinearVelocity(btVector3(0,0,0));
	m_carChassis -> setAngularVelocity(btVector3(0,0,0));
}

/******************************************************************
  Function:   destroyVehicle
  Purpose:    clean up
******************************************************************/
void Vehicle::destroyVehicle()
{
	if (car3DModel) delete car3DModel;
	//cleanup in the reverse order of creation/initialization
	delete m_carChassis;
	delete vehicleMotionState;

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	delete m_vehicleRayCaster;
	delete m_vehicle;
}

/******************************************************************
  Function:   loadVehicle
  Purpose:    Initiate loading process of a 3D vehicle model
******************************************************************/
void Vehicle::loadVehicle(const char * vName)
{
	// set directory of model
	modelDir = "data/cars/" + std::string(vName) + "/";
	// string to .dat file
	std::string fullLocation = modelDir + std::string(vName) + ".dat";

	// initiate loading process
	loadDataFile(fullLocation.c_str());
}

/******************************************************************
  Function:   loadDataFile
  Purpose:    load requested .dat file
              the .dat file contains necessary properties
				  for a vehicle model to work and look as intended
				  in the game.
              The format of the .dat file is straight forward.
              The comments below should explain what the file contains
******************************************************************/
void Vehicle::loadDataFile(const char * datFileName)
{
	char modelFileName[256];
	std::ifstream in(datFileName);

	if (in.fail())
	{
		RacerGameApp::closeApplicationFailure("Could not load model .dat file\n");
	}

	// read file name of 3ds model
	in >> modelFileName;
	// read number of textures 3ds model needs
	in >> numTextures;

	// allocate space for model texture names and IDs
	if (numTextures != 0)
	{
		carTextureNames.resize(numTextures);
	}

	// read texture filenames from data file
	for (int i = 0; i < numTextures; ++i)
	{
		in >> carTextureNames[i];
	}

	// read size scale of model
	in >> carScale;
	// read model transformation
	in >> modelTrans[0];         // x
	in >> modelTrans[1];         // y
	in >> modelTrans[2];         // z
	// read model rotation
	in >> modelRotate[0];        // angle
	in >> modelRotate[1];        // x
	in >> modelRotate[2];        // y
	in >> modelRotate[3];        // z

	// now read bounding box dimensions
	in >> carDims[0];            // x
	in >> carDims[1];            // y
	in >> carDims[2];            // z

	// finally read wheel Mesh ID in model file
	// these will be used to orient the wheel.
	// Note: at this point these variables are useless because
	//       there was not much time to implement
	in >> wheelFL_ID;
	in >> wheelFR_ID;
	in >> wheelRL_ID;
	in >> wheelRR_ID;

	in.close();

	if (modelName != "default")
	{
		std::string fullLocation = modelDir + (std::string)modelFileName;
		loadVehicleModel(fullLocation.c_str());
	}
	loadVehicleModelTextures();
	if (modelName != "default") createDisplayList();
}

/******************************************************************
  Function:   loadVehicleModel
  Purpose:    Load 3ds file
******************************************************************/
void Vehicle::loadVehicleModel(const char * mName)
{
	// if there is a model object already, we delete it
	if (car3DModel) delete car3DModel;

	// continue loading
	car3DModel = new C3DSObject();

	char *file = new char[strlen(mName)+1];
	strcpy ( file, mName );

	car3DModel -> LoadModel(file);
}

/******************************************************************
  Function:   loadVehicleModelTextures
  Purpose:    load vehicle textures
******************************************************************/
void Vehicle::loadVehicleModelTextures()
{
	//use the names of the textures we retrieved from .dat file to load them
	for (int i = 0; i < numTextures; ++i)
	{
		std::string fullLocation = modelDir + carTextureNames[i];
		char * texName= new char [fullLocation.size() + 1];
		strcpy (texName, fullLocation.c_str());

		// use DevILs ability to load an image and generate OpenGL textures
		// by making the OpenGL calls for us. the texture id is returned
		texNamesMap[carTextureNames[i]] = ilutGLLoadImage(texName);
	}
}

/******************************************************************
  Function:   createDisplayList
  Purpose:    Since vehicle models are also static we can
              put them in a display list
******************************************************************/
void Vehicle::createDisplayList()
{
	vehicleDL = glGenLists(1);

	glNewList(vehicleDL,GL_COMPILE);
		// Loop through all meshes that are apart of the file and render them.
		for(int i = 0; i < car3DModel->totalMeshes; i++)
		//for(int i = 9; i < 13; i++)
		{
			char * tName = car3DModel->materialList[car3DModel->meshList[i].pFaces[0].matId].textureName;
			if (numTextures > 0 && strlen(tName) > 1)
			{
				glEnable(GL_TEXTURE_2D);
				std::string texName = tName;
				glBindTexture(GL_TEXTURE_2D, texNamesMap[texName]);
			}

			glBegin(GL_TRIANGLES);
			//printf("mesh: %d\n", i);
			// each mesh is composed of several triangular faces. Draw them
			for(unsigned int f = 0; f < car3DModel->meshList[i].totalFaces; f++)
			{
				// Get pointers to make the below code cleaner.
				stMesh *pMesh = &car3DModel->meshList[i];
				stFace *pFace = pMesh->pFaces;
				stTexCoord *pTexCoord = pMesh->pTexCoords;
				unsigned char colorR  = car3DModel->materialList[pFace[f].matId].color.r;
				unsigned char colorG  = car3DModel->materialList[pFace[f].matId].color.g;
				unsigned char colorB  = car3DModel->materialList[pFace[f].matId].color.b;

				// If one or both or NULL, we got a problem.
				if(!pMesh || !pFace) continue;

				// Draw the triangle.
				glNormal3f(pFace[f].normal.x, pFace[f].normal.y, pFace[f].normal.z);
				//printf("normals: %f, %f, %f\n", pFace[f].normal.x, pFace[f].normal.y, pFace[f].normal.z);

				glColor3ub(colorR, colorG, colorB);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[0]].tu, pTexCoord[pFace[f].indices[0]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[0]].x,
							pMesh->pVertices[pFace[f].indices[0]].y,
							pMesh->pVertices[pFace[f].indices[0]].z);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[1]].tu, pTexCoord[pFace[f].indices[1]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[1]].x,
							pMesh->pVertices[pFace[f].indices[1]].y,
							pMesh->pVertices[pFace[f].indices[1]].z);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[2]].tu, pTexCoord[pFace[f].indices[2]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[2]].x,
							pMesh->pVertices[pFace[f].indices[2]].y,
							pMesh->pVertices[pFace[f].indices[2]].z);
			}
			glEnd();
         glDisable(GL_TEXTURE_2D);
		}
	glEndList();
}

/******************************************************************
  Function:   changeVehicleModel
  Purpose:    Provide ability to change vehicle models on the fly
              (For demonstrational purposes)
              ideally this is not necessarily desired.
******************************************************************/
void Vehicle::changeVehicleModel(const char * newModelName)
{
	// delete any old textures
	for (int i = 0; i < numTextures; ++i)
	{
		GLuint tex = texNamesMap[carTextureNames[i]];
		glDeleteTextures(1, &tex);
	}

	numTextures = 0;

	modelName = std::string(newModelName);

	// and load a new model
	loadVehicle(modelName.c_str());
}

/******************************************************************
  Function:   setStartingLocation
  Purpose:    set the location where vehicle should start
******************************************************************/
void Vehicle::setStartingLocation(float * loc)
{
	startPoint[0] = loc[0];
	startPoint[1] = loc[1];
	startPoint[2] = loc[2];
}

/******************************************************************
  Function:   setStartingLocation
  Purpose:    set the orientation of vehicle when starting
******************************************************************/
void Vehicle::setStartingOrientation(float * orientation)
{
	startOrient[0] = orientation[0];
	startOrient[1] = orientation[1];
	startOrient[2] = orientation[2];
	startOrient[3] = orientation[3];
}

/******************************************************************
  Function:   updateVehicle
  Purpose:    apply forces to vehicle for movement
******************************************************************/
void Vehicle::updateVehicle()
{
	// Set back wheels steering value
	int wheelIndex = 2;
	m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
	m_vehicle->setBrake(gBreakingForce,wheelIndex);

	wheelIndex = 3;
	m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
	m_vehicle->setBrake(gBreakingForce,wheelIndex);

	// update front wheels steering value
	if (steerRight)
	{
		gVehicleSteering -= steeringIncrement;
		if (gVehicleSteering < -steeringClamp) gVehicleSteering = -steeringClamp;
	}
	else if (steerLeft)
	{
		gVehicleSteering += steeringIncrement;
		if (gVehicleSteering > steeringClamp) gVehicleSteering = steeringClamp;
	}
	else
	{
		gVehicleSteering = 0.0;
	}

	// Set front wheels steering value
	wheelIndex = 0;
	m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);

	wheelIndex = 1;
	m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
}

/******************************************************************
  Function:   applyBrakingForce
  Purpose:    make vehicle brake
******************************************************************/
void Vehicle::applyBrakingForce(bool apply)
{
	if (apply) {
		gBreakingForce = maxBreakingForce;
	} else {
		gBreakingForce = 0.f;
	}
}

/******************************************************************
  Function:   applyAccelerationForce
  Purpose:    make vehicle accelerate
******************************************************************/
void Vehicle::applyAccelerationForce(bool apply)
{
	if (apply) {
		if (!isReverse) gEngineForce = maxEngineForce;
		else gEngineForce = -maxEngineForce;
		gBreakingForce = 0.f;
	} else {
		gEngineForce = 0.f;
	}
}

/******************************************************************
  Function:   applySteeringRight
  Purpose:    make vehicle turn right
******************************************************************/
void Vehicle::applySteeringRight(bool apply)
{
	steerRight = apply;
}

/******************************************************************
  Function:   applySteeringLeft
  Purpose:    make vehicle turn left
******************************************************************/
void Vehicle::applySteeringLeft(bool apply)
{
	steerLeft = apply;
}

/******************************************************************
  Function:   toggleReverse
  Purpose:    Set car in reverse or drive
******************************************************************/
void Vehicle::toggleReverse()
{
	isReverse = (isReverse? false : true);
}

void Vehicle::renderVehicle()
{
	if (modelName == "default")
	{
		renderVehicleDefault();
	}
	else
	{
		renderVehicleModel();
	}
}

/******************************************************************
  Function:   renderVehicleModel
  Purpose:    render the uploaded 3ds model
******************************************************************/
void Vehicle::renderVehicleModel()
{
	btTransform trans;
	float m[16];

	// get the transformation of car from physics engine
	vehicleMotionState->getWorldTransform(trans);
	trans.getOpenGLMatrix(m);

	glPushMatrix();
		glMultMatrixf(m);
		glTranslatef(modelTrans[0],modelTrans[1], modelTrans[2]);
		glScalef(carScale,carScale,carScale);
		glRotatef(modelRotate[0],modelRotate[1],modelRotate[2],modelRotate[3]);

		glCallList(vehicleDL);
	glPopMatrix();
}

/******************************************************************
  Function:   renderVehicleDefault
  Purpose:    render the default vehicle
******************************************************************/
void Vehicle::renderVehicleDefault()
{
	glDisable(GL_RESCALE_NORMAL);
	btTransform trans;
	float m[16];

	vehicleMotionState->getWorldTransform(trans);
	trans.getOpenGLMatrix(m);

	glColor3f(0.1,0.1,0.1);
	// draw wheels
	renderVehicleDefaultWheel(0);
	renderVehicleDefaultWheel(1);
	renderVehicleDefaultWheel(2);
	renderVehicleDefaultWheel(3);

	// draw chassis
	glColor3f(0.1,0.5,0.1);
	glPushMatrix();
		glMultMatrixf(m);
		glTranslatef(0.0,2.0,0.0);
		glScalef(2.4,2.0,8.1);
		glutSolidCube(1.0);
	glPopMatrix();
	glEnable(GL_RESCALE_NORMAL);
}

/******************************************************************
  Function:   renderVehicleDefaultWheel
  Purpose:    draw the default vehicle's wheels
******************************************************************/
void Vehicle::renderVehicleDefaultWheel(int i)
{
	float m[16];
	GLUquadricObj *quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, (GLenum)GLU_FILL);
	gluQuadricNormals(quadObj, (GLenum)GLU_SMOOTH);

	//synchronize the wheels with the (interpolated) chassis worldtransform
	m_vehicle->updateWheelTransform(i,true);
	//draw wheels (cylinders)
	m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);

	glPushMatrix();
		glMultMatrixf(m);
		glRotatef(90.0, 0.0, 1.0, 0.0);

		glPushMatrix();
			glTranslatef(0.0, 0.0, wheelWidth/2);
			gluDisk(quadObj,0,wheelRadius,15, 10);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texNamesMap["tire.tga"]);
			gluQuadricTexture(quadObj, true);
			glTranslatef(0.0, 0.0, -wheelWidth);
			gluCylinder(quadObj, wheelRadius, wheelRadius, wheelWidth, 15, 10);
			gluQuadricTexture(quadObj, false);
			glDisable(GL_TEXTURE_2D);

			glRotatef(-180.0, 0.0, 1.0, 0.0);
			gluDisk(quadObj,0,wheelRadius,15, 10);
		glPushMatrix();
	glPopMatrix();
	gluDeleteQuadric(quadObj);
}

/******************************************************************
  Function:   resetScene
  Purpose:    reset variables
******************************************************************/
void Vehicle::resetScene()
{
	if (m_vehicle)
	{
		m_vehicle -> resetSuspension();
		for (int i = 0; i < m_vehicle->getNumWheels(); i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle -> updateWheelTransform(i, true);
		}
	}
}
