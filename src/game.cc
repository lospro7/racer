/**************************************************************************

  File: game.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/

#include <iostream>
#include <cstdlib>
#include <IL/ilut.h>
#include "game.h"

// game objects
GameWorld * RacerGameApp::currentGame = NULL;

// initial values
int RacerGameApp::appMode = APP_GAME_MODE;
GLsizei RacerGameApp::windowWidth  = 800;
GLsizei RacerGameApp::windowHeight = 600;

/******************************************************************
  Function:   startNewGame
  Purpose:    starts a new game
******************************************************************/
void RacerGameApp::startNewGame()
{
	currentGame = new GameWorld();
}

/******************************************************************
  Function:   setAppMode
  Purpose:    keep track of where the user is in the application
******************************************************************/
void RacerGameApp::setAppMode(int mode)
{
	appMode = mode;
}

/******************************************************************
  Function:   displayHandler
  Purpose:    This is the display callback required by glut
******************************************************************/
void RacerGameApp::displayHandler()
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> renderWorld();
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   idleHandler
  Purpose:    This is the idle callback
******************************************************************/
void RacerGameApp::idleHandler()
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> updateWorld();
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   reshapeHandler
  Purpose:    This is the reshape window callback
******************************************************************/
void RacerGameApp::reshapeHandler(int width, int height)
{
	//adjust window dimension globals
	windowWidth  = width;
	windowHeight = height;

	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> reshapeWorld(width, height);
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   keySpecialDownHandler
  Purpose:    special key down callback
******************************************************************/
void RacerGameApp::keySpecialDownHandler(int key, int x, int y)
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> keySpecialDownEvent(key, x, y);
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   keySpecialDownHandler
  Purpose:    special key up callback
******************************************************************/
void RacerGameApp::keySpecialUpHandler(int key, int x, int y)
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> keySpecialUpEvent(key, x, y);
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   keySpecialDownHandler
  Purpose:    regular key down callback
******************************************************************/
void RacerGameApp::keyRegularDownHandler(unsigned char key, int x, int y)
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> keyRegularDownEvent(key, x, y);
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

/******************************************************************
  Function:   keySpecialDownHandler
  Purpose:    regular key up callback
******************************************************************/
void RacerGameApp::keyRegularUpHandler(unsigned char key, int x, int y)
{
	switch (appMode)
	{
		case APP_GAME_MODE:
			currentGame -> keyRegularUpEvent(key, x, y);
			break;
		case APP_GAME_GUI_MODE:
			break;
		case APP_GUI_MODE:
		default:
			break;
	}
}

// program initialization and termination functions
/******************************************************************
  Function:   initApplication
  Purpose:    Application, OpenGL, and GLUT initializations
******************************************************************/
void RacerGameApp::initApplication()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glutInitWindowPosition(0,0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Racer");

	//Callback routines
	//glutIgnoreKeyRepeat(1);
	glutDisplayFunc(displayHandler);
	glutIdleFunc(idleHandler);
	glutReshapeFunc(reshapeHandler);

	//Mouse and keyboard callbacks
	glutSpecialFunc(keySpecialDownHandler);
	glutSpecialUpFunc(keySpecialUpHandler);
	glutKeyboardFunc(keyRegularDownHandler);
	glutKeyboardUpFunc(keyRegularUpHandler);

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	startNewGame();
}

void RacerGameApp::closeApplication()
{
	// clean up after ourselves
	if (currentGame) delete currentGame;
	currentGame = NULL;
}

/******************************************************************
  Function:   closeApplicationSuccess
  Purpose:    function to execute when user asks to close application
******************************************************************/
void RacerGameApp::closeApplicationSuccess()
{
	closeApplication();

	exit(EXIT_SUCCESS); // happily exit
}

/******************************************************************
  Function:   closeApplicationFailure
  Purpose:    function to execute when an error occurs and
              the application needs to close. Prints a message.
******************************************************************/
void RacerGameApp::closeApplicationFailure(const char * msg)
{
	// attempt to clean up after ourselves after error
	closeApplication();

	std::cout << "An error occured and the application needs to close" << std::endl;
	std::cout << "Message: " << msg;        // print error message
	exit(EXIT_FAILURE); // bad place to get here
}
