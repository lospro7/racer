# Makefile for final project
# CS425 - Computer Graphics
# Created by Carlos Espinoza

# includes and libraries
INCLUDE = -I/usr/include/ -Ibullet/src -Iinc/

# compiler flags and linking libraries
COMPILERFLAGS = -Wall -g
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -Lbullet/src -lBulletDynamics -lBulletCollision -lLinearMath -L/usr/X11R6/lib -lX11 -lXi -lXmu -lGL -lGLU  -lglut -lm -L/usr/lib -lIL -lILU -lILUT

SRCPATH=src/
OBJPATH=obj/
SRCEXT=.cc
OBJEXT=.o

vpath %$(SRCEXT) $(SRCPATH)

MODULES=main game gameWorld 3dsObject physics vehicle vehicleContainer track GLDebugDrawer
OBJS:=$(addprefix $(OBJPATH),$(MODULES))
OBJS:=$(addsuffix $(OBJEXT),$(OBJS))

MAIN = Racer

all: $(MAIN)
	@echo Compilation successful

$(MAIN): mkdirs $(OBJS)
	@echo Linking...
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBRARIES)

$(OBJPATH)%$(OBJEXT): %$(SRCEXT)
	$(CC) $(CFLAGS) -c $(SRCPATH)$(*F)$(SRCEXT) -o $@

mkdirs:
	mkdir -p obj

clean:
	$(RM) $(OBJS) $(MAIN)

