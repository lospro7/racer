/**************************************************************************

  File: vehicles.h

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza
  carlospro7@gmail.com

**************************************************************************/
#ifndef _VEHICLE_H_
#define _VEHICLE_H_

class btVehicleTuning;
struct btVehicleRaycaster;
class btCollisionShape;

#include <iostream>
#include <map>
#include <string>
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "game.h"
#include "3dsObject.h"

class RacerGameApp;

class Vehicle
{
public:
	Vehicle();
	~Vehicle() { destroyVehicle(); }
	void initVehicle(const char *newVehicleName);
	void destroyVehicle();

	void loadVehicle(const char * vehicleName);
	void loadDataFile(const char * datFileName);
	void loadVehicleModel(const char * mName);
	void loadVehicleModelTextures();
	void setStartingLocation(float * loc);
	void setStartingOrientation(float * orientation);
	void createDisplayList();

	void changeVehicleModel(const char * datFileName);

	void updateVehicle();
	void applyBrakingForce(bool);
	void applyAccelerationForce(bool);
	void applySteeringRight(bool);
	void applySteeringLeft(bool);
	void toggleReverse();

	void resetScene();
	void renderVehicle();
	void renderVehicleModel();
	void renderVehicleDefault();
	void renderVehicleDefaultWheel(int i);

	btDefaultMotionState* getMotionState() { return vehicleMotionState; }
	btRigidBody* getVehicleRigidBody() { return m_carChassis; }
	btVehicleRaycaster* getRaycaster() { return m_vehicleRayCaster; }
	btRaycastVehicle*	getRaycast() { return m_vehicle; }

	float getWheelRadius() { return wheelRadius; }
	float getWheelWidth() { return wheelWidth; }
	btScalar getSuspensionRestLength() { return suspensionRestLength; }
	float getSuspensionStiffness() { return suspensionStiffness; }
	float getSuspensionDamping() { return suspensionDamping; }
	float getSuspensionCompression() { return suspensionCompression; }
	float getWheelFriction() { return wheelFriction; }
	float getRollInfluence() { return rollInfluence; }

	btRaycastVehicle::btVehicleTuning m_tuning; // uses wheel properties
	btVehicleRaycaster* m_vehicleRayCaster;     // simulates four wheels
	btRaycastVehicle* m_vehicle;                // vehicle raycast object

private:
	// physics engine essentials
	static btCollisionShape* vehicleColShape;   // collision shape
	btDefaultMotionState* vehicleMotionState;   // motion state, used for retrieving vehicle location in world

	btRigidBody* m_carChassis;                  // rigid body object
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes; // list of collision shapes used

	// 3ds model stuff
	std::string modelName;         // name of 3D model
	C3DSObject * car3DModel;       // 3ds model object
	std::map<std::string, GLuint> texNamesMap; // map a model texture name to an OpenGL texture name
	std::vector<std::string> carTextureNames;  // contains names of textures in model
	int numTextures;               // number of textures
	// The following are used to orient wheel meshes
	int wheelFL_ID;                // Front Left wheel ID in model
	int wheelFR_ID;                // Front Right wheel ID in model
	int wheelRL_ID;                // Rear Left wheel ID in model
	int wheelRR_ID;                // Rear Right wheel ID in model
	float startPoint[3];           // where car starts
	float startOrient[4];          // start orientation
	std::string modelDir;          // directory where model is located
	// some models need to be shifted, oriented, and scaled
	float modelTrans[3];
	float modelRotate[4];
	float carScale;                // how big to scale the model
	float carDims[3];              // bounding box dimensions
	GLuint vehicleDL;

	// vehicle properties
	float	gVehicleSteering;
	float	steeringIncrement;   // How fast to steer
	float	steeringClamp;       // steering value limit

	float	gEngineForce;        // force to apply to engine
	float	gBreakingForce;      // breaking force
	float	maxEngineForce;      // max engine force to apply
	float	maxBreakingForce;    // max breaking force

	float	wheelRadius;
	float	wheelWidth;
	btScalar suspensionRestLength;

	// wheel properties. Should be self explanatory
	float	suspensionStiffness;
	float	suspensionDamping;
	float	suspensionCompression;
	float	wheelFriction;
	//shifts center of mass to make car less/more prone to rolling over
	float	rollInfluence;

	// steering properties
	bool steerRight;
	bool steerLeft;
	bool isSteering;
	bool isReverse;
};

#endif //_VEHICLE_H_
