/**************************************************************************

  File: game.h

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/
#ifndef _RACER_GAME_H_
#define _RACER_GAME_H_

#define APP_GUI_MODE       1
#define APP_GAME_MODE      2
#define APP_GAME_GUI_MODE  3

#include <GL/glut.h>
#include "gameWorld.h"

class GameWorld;

class RacerGameApp
{
public:
	static void startNewGame();
	static void gameLoop();

	static void setAppMode(int);
	static int getAppMode() { return appMode; }

	// callback handlers
	static void displayHandler();
	static void idleHandler();
	static void reshapeHandler(int, int);

	// keyboard input handlers
	static void keySpecialDownHandler(int key, int x, int y);
	static void keySpecialUpHandler(int key, int x, int y);
	static void keyRegularDownHandler(unsigned char key, int x, int y);
	static void keyRegularUpHandler(unsigned char key, int x, int y);

	// application initialization and termination
	static void initApplication();
	static void closeApplication();
	static void closeApplicationSuccess();
	static void closeApplicationFailure(const char *);

private:
	static int appMode;
	static GLsizei windowWidth;
	static GLsizei windowHeight;
	static GameWorld* currentGame;
};

#endif //_RACER_GAME_H
