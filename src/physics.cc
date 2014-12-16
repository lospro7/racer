/**************************************************************************

  File: physics.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/

#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "physics.h"

Physics::Physics(float grav)
{
	initSimulation(grav);
}

void Physics::initSimulation(float grav)
{
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	int maxProxies = 1024;

	//set broadphase algorith for collision detection
	broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	// bullet engine initialization
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;

	//create a new dynamics world and set gravity
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,grav,0));
}

#define CUBE_HALF_EXTENTS 1
void Physics::registerVehicle(Vehicle* vehicle)
{
	btVector3 wheelDirectionCS0(0,-1,0);
	btVector3 wheelAxleCS(-1,0,0);
	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;
	float	wheelRadius = vehicle -> getWheelRadius();
	float	wheelWidth = vehicle -> getWheelWidth();
	btScalar sRestLength = vehicle -> getSuspensionRestLength();

	dynamicsWorld -> getBroadphase()-> getOverlappingPairCache() ->
		cleanProxyFromPairs(
			vehicle -> getVehicleRigidBody() -> getBroadphaseHandle(),
			dynamicsWorld -> getDispatcher()
		);

	dynamicsWorld -> addRigidBody (vehicle -> getVehicleRigidBody());

	/// create vehicle
	vehicle->m_vehicleRayCaster = new btDefaultVehicleRaycaster(dynamicsWorld);
	vehicle->m_vehicle = new btRaycastVehicle(
		vehicle->m_tuning, vehicle->getVehicleRigidBody(), vehicle->getRaycaster());

	///never deactivate the vehicle
	vehicle -> getVehicleRigidBody() -> setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld -> addVehicle(vehicle -> m_vehicle);

	float connectionHeight = 1.2f;

	//choose coordinate system
	vehicle->getRaycast() -> setCoordinateSystem(rightIndex,upIndex,forwardIndex);

	bool isFrontWheel=true;
	btVector3 connectionPointCS0(1.8*CUBE_HALF_EXTENTS-(1*wheelWidth),connectionHeight,3.7*CUBE_HALF_EXTENTS-wheelRadius);
	vehicle->getRaycast() -> addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,sRestLength,wheelRadius,vehicle -> m_tuning,isFrontWheel);

	connectionPointCS0 = btVector3(-1.8*CUBE_HALF_EXTENTS+(1*wheelWidth),connectionHeight,3.7*CUBE_HALF_EXTENTS-wheelRadius);
	vehicle->getRaycast() -> addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,sRestLength,wheelRadius,vehicle -> m_tuning,isFrontWheel);

	isFrontWheel = false;
	connectionPointCS0 = btVector3(-1.8*CUBE_HALF_EXTENTS+(1*wheelWidth),connectionHeight,-3*CUBE_HALF_EXTENTS+wheelRadius);
	vehicle->getRaycast() -> addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,sRestLength,wheelRadius,vehicle -> m_tuning,isFrontWheel);

	connectionPointCS0 = btVector3(1.8*CUBE_HALF_EXTENTS-(1*wheelWidth),connectionHeight,-3*CUBE_HALF_EXTENTS+wheelRadius);
	vehicle->getRaycast() -> addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,sRestLength,wheelRadius,vehicle -> m_tuning,isFrontWheel);

	for (int i = 0; i < vehicle->getRaycast()->getNumWheels(); i++)
	{
		btWheelInfo& wheel = vehicle -> getRaycast() -> getWheelInfo(i);
		wheel.m_suspensionStiffness = vehicle -> getSuspensionStiffness();
		wheel.m_wheelsDampingRelaxation = vehicle -> getSuspensionDamping();
		wheel.m_wheelsDampingCompression = vehicle -> getSuspensionCompression();
		wheel.m_frictionSlip = vehicle -> getWheelFriction();
		wheel.m_rollInfluence = vehicle -> getRollInfluence();
	}
}

void Physics::registerMap(RaceTrack * racetrack)
{
	float * heightData = racetrack -> getHeightfieldData();
	int w = racetrack -> getHeightfieldWidth();
	int l = racetrack -> getHeightfieldLength();
	float hMin = racetrack -> getMinHeight();
	float hMax = racetrack -> getMaxHeight();

	bool flipQuadEdges = true;
	btHeightfieldTerrainShape * heightfieldShape =
		new btHeightfieldTerrainShape(
			w, l,
			heightData,
			btScalar(1.0f),
			btScalar( hMin ),
			btScalar( hMax+10 ),
			1,
			PHY_FLOAT,
			flipQuadEdges
		);

	btAssert(heightfieldShape && "null heightfield");

	// scale the shape
	heightfieldShape->setLocalScaling(btVector3(16,1,16));

	// stash this shape away
	collisionShapes.push_back(heightfieldShape);

	// set origin to middle of heightfield
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,50,0));

	// create ground object
	float mass = 0.0;
	racetrack -> groundShape = heightfieldShape;
	racetrack -> groundMotionState = new btDefaultMotionState(tr);
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(mass,racetrack -> groundMotionState,heightfieldShape,btVector3(0,0,0));
	racetrack -> groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld -> addRigidBody(racetrack -> groundRigidBody);
}

void Physics::stepSimulation(float timeStep)
{
	dynamicsWorld->stepSimulation(timeStep,7);
}

void Physics::cleanSimulation()
{
	// cleaning up after ourselves
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}
