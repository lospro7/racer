# Getting started

There are a few things you will need.

OpenGL
DevIL (Developers Image Library) 1.7.8
Bullet Physics Engine 2.76
GNU C++ compiler. Haven't tried it with windows compilers yet.
GLUT (window management and event handling). freeglut3 should work.

DevIL is a must because it handles the texture loading portions. I chose this for this project instead of writing it myself because of the time constraints. 

You need to download Bullet 2.76. Then compile it according to the manual and afterwards, you need to copy the 'src' folder from the Bullet build directly into the 'bullet' directory that is in the game folder. I've left a copy of my 'bullet/src' directory in the build as an example, but I encourage you to build it and copy it yourself as our platforms are most likely not the same.

Install freeglut3 (from the linux repository) and DevIL (from http://openil.sourceforge.net/), and you should be okay--so long as you are using linux. If you are using Windows, you might have to do some extra work to make sure it compiles. The external tools like DevIL, GLUT and Bullet are available in Windows so that may not be the problem: linking them together might be it. If you choose the Windows path, you are on your own.

There is a Makefile and a readme file.
If all is well, you should be able to type 'make' into your command line and run the game
You can modify the make file if you need to change include and library paths.
The readme file contains instructions on how to play the game.

Good luck I hope you get it working quickly.

# Controls

## Keys
* 1 to 6 - Change Vehicle Models
* F1 - Vehicle view
* F2 - Free camera

## Vehicle view Controls
* Up - Forward
* Down - Break
* Left - Steer left
* Right - Steer Right
* Z - Reverse/Drive

## Free camera controls
* W - move forward
* S - move backward
* A - Strafe left
* D - Strafe right
* Up - Look up
* Down - Look down
* Left - Look left
* Right - Look right

## Other
* G - loop through debug modes
* Q - quit
