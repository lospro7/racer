/**************************************************************************

  File: gameWorld.h

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/
#ifndef _RACER_GAME_WORLD_H_
#define _RACER_GAME_WORLD_H_

#include "GLDebugDrawer.h"
#include "physics.h"
#include "track.h"
#include "vehicle.h"

#define CAMERA_CAR         10
#define CAMERA_FREE        11

#define DEBUG_MODE_1       20
#define DEBUG_MODE_2       21
#define DEBUG_MODE_3       22
#define DEBUG_MODE_4       23

class Physics;
class Vehicle;
class RaceTrack;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void initWorld();
	void destroyWorld();

	void updateCamera();
	void renderWorld();
	void updateWorld();
	void reshapeWorld(int,int);

	//free camera
	void updateCameraLocation();
	void updateCameraOrientation();

	// input stuff
	void keySpecialDownEvent(int key, int x, int y);
	void keySpecialUpEvent(int key, int x, int y);
	void keyRegularDownEvent(unsigned char key, int x, int y);
	void keyRegularUpEvent(unsigned char key, int x, int y);

	// Debugging
	void nextDebugMode();

	// temporary functions
	// used for demonstration
	void tempInitSimulate();
	void tempLoadTextures();
	void drawTempObjects();
	void createBoxDL();
	void deleteTempObjects();

private:
	int raceMapDL;
	int cameraMode;
	RaceTrack * raceMap;
	Physics * worldPhysics;
	Vehicle * myVehicle;

	GLDebugDrawer * gDebugDrawer;
	int debugMode;
	btVector3 m_cameraPosition;
	btVector3 m_cameraTargetPosition;
	float m_cameraHeight;
	float m_minCameraDistance;
	float m_maxCameraDistance;
};

#endif //_RACER_GAME_H_
