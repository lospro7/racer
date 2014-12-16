# Getting started

A few dependencies you will need.

* OpenGL
* DevIL (Developers Image Library) 1.7.8 (http://openil.sourceforge.net/)
* Bullet Physics Engine 2.76
* GNU C++ compiler. Haven't tried it with windows compilers yet.
* GLUT (window management and event handling). freeglut3 should work.

Bullet is included in this build, but you may have to download it and built it yourself.

# Compiling
I've only tested this in an Ubuntu linux machine.

```
# make
```

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
