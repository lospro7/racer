/**************************************************************************

  File: physics.h

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/
#ifndef _RACER_PHYSICS_H_
#define _RACER_PHYSICS_H_

#include "track.h"
#include "vehicle.h"

class Vehicle;
class RaceTrack;

class Physics
{
public:
	Physics(float);
	~Physics() { cleanSimulation(); }
	void initSimulation(float);
	void stepSimulation(float);
	void cleanSimulation();

	void registerVehicle(Vehicle *);
	void registerMap(RaceTrack *);

	btDynamicsWorld* getDynamicsWorld() { return dynamicsWorld; }

private:
	//bullet engine stuff
	btAxisSweep3* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
};

#endif //_RACER_PHYSICS_H_
