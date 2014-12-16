/**************************************************************************

  File: main.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/

#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include "GLDebugDrawer.h"
#include "game.h"

int main(int argc, char **argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	RacerGameApp::initApplication();
	glutMainLoop();

	return EXIT_SUCCESS;
}
