/**************************************************************************

  File: track.h

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/
#ifndef _RACER_TRACK_H_
#define _RACER_TRACK_H_

#include <iostream>
#include <map>
#include <string>
#include <btBulletDynamicsCommon.h>
#include <GL/glut.h>
#include "game.h"
#include "3dsObject.h"

class RacerGameApp;

class RaceTrack
{
public:
	RaceTrack(float, float);
	~RaceTrack() { cleanUpRaceTrack(); }
	void cleanUpRaceTrack();

	void loadMap(char *fileName);
	void loadModelFile(char * datFileName);
	void loadRaceTrackTextures();
	void loadHeightMap();
	void createDisplayList();

	float * getHeightfieldData() { return rawHeightfieldData; }
	float getMinHeight() { return minHeight; }
	float getMaxHeight() { return maxHeight; }
	int getHeightfieldWidth() { return heightfieldWidth; }
	int getHeightfieldLength() { return heightfieldLength; }

	void renderRaceTrack();
	float * getStartingLocation() { return playerStartingLocation; }
	float * getStartingOrientation() { return playerStartOrient; }

	void setMotionState();

	btDefaultMotionState* getMotionState() { return groundMotionState; }

	// bullet physics stuff
	btCollisionShape* groundShape;
	btDefaultMotionState* groundMotionState;
	btRigidBody* groundRigidBody;

private:
	GLuint mapDisplayListID;

	float * rawHeightfieldData;
	float minHeight, maxHeight;
	int heightfieldWidth, heightfieldLength;

	C3DSObject track3DModel;       // 3ds model object
	std::map<std::string, GLuint> texNamesMap;
	std::vector<std::string> raceTrackTextureNames; // contains names of textures in model
	int numTextures;               // number of textures
	std::string dir;

	//initial locations
	float playerStartingLocation[3];
	float playerStartOrient[4];
};

// struct used to sort height field data
struct Ordering_Functor
{
	bool operator()(const btVector3& a, const btVector3& b)
	{
		if (a.x() == b.x())
			return a.y() < b.y();
		else
			return a.x() < b.x();
	}
};

#endif //_RACER_TRACK_H_
