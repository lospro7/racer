/**************************************************************************

  File: track.cc

  Racer
  CS425 Computer Graphics - Final Project

  Developed by Carlos Espinoza

**************************************************************************/

#include <fstream>
#include <vector>
#include <algorithm>
#include <GL/glut.h>
#include <IL/ilut.h>
#include "track.h"

/******************************************************************
  Function:   constructor
  Purpose:
******************************************************************/
RaceTrack::RaceTrack(float hMin, float hMax)
{
	playerStartingLocation[0] = 0.0;
	playerStartingLocation[1] = 0.0;
	playerStartingLocation[2] = 0.0;

	minHeight = maxHeight = 0.0;
	rawHeightfieldData = NULL;
	heightfieldWidth = heightfieldLength = 0;
	mapDisplayListID = 0;
	dir = "data/tracks/track1/";
}

/******************************************************************
  Function:   loadMap
  Purpose:    load map
******************************************************************/
void RaceTrack::loadMap(char * fileName)
{
	std::string fullLocation = dir + std::string(fileName);
	char * file = new char [fullLocation.size() + 1];
	strcpy (file, fullLocation.c_str());

	loadModelFile(file);
}

/******************************************************************
  Function:   loadModelFile
  Purpose:    load requested .dat file
              the .dat file contains necessary properties
				  for a vehicle model to work and look as intended
				  in the game
******************************************************************/
void RaceTrack::loadModelFile(char * datFileName)
{
	char modelFileName[256];
	std::ifstream in(datFileName);

	if (in.fail())
	{
		RacerGameApp::closeApplicationFailure("Could not load race track model .dat file\n");
	}

	// read file name of 3ds model
	in >> modelFileName;
	// read number of textures 3ds model needs
	in >> numTextures;

	// allocate space for model texture names and IDs
	if (numTextures != 0)
	{
		raceTrackTextureNames.resize(numTextures);
	}

	// read texture filenames from data file
	for (int i = 0; i < numTextures; ++i)
	{

		in >> raceTrackTextureNames[i];
		//std::cout << "From .dat file: "<< raceTrackTextureNames[i] << std::endl;
	}

	// height map min and max heights
	in >> minHeight;
	in >> maxHeight;

	// height field width and length
	in >> heightfieldWidth;
	in >> heightfieldLength;

	// player starting location
	in >> playerStartingLocation[0];
	in >> playerStartingLocation[1];
	in >> playerStartingLocation[2];
	// and the direction the player faces at the start
	in >> playerStartOrient[0];
	in >> playerStartOrient[1];
	in >> playerStartOrient[2];
	in >> playerStartOrient[3];

	std::string fullLocation = dir + (std::string)modelFileName;
	char * file = new char [fullLocation.size() + 1];
	strcpy (file, fullLocation.c_str());

	if (track3DModel.LoadModel(file))
	{
      loadRaceTrackTextures();
      loadHeightMap();
      createDisplayList();
	}
	else exit(-1);
}

/******************************************************************
  Function:   loadRaceTrackTextures
  Purpose:    load vehicle textures
******************************************************************/
void RaceTrack::loadRaceTrackTextures()
{
	//use the names we retrieved to open the textures and load them
	for (int i = 0; i < numTextures; ++i)
	{
		std::string fullLocation = dir + raceTrackTextureNames[i];
		char * texName= new char [fullLocation.size() + 1];
		strcpy (texName, fullLocation.c_str());

		// use DevILs ability to load an image and generate OpenGL textures
		// by making the OpenGL calls for us. the texture id is returned
		texNamesMap[raceTrackTextureNames[i]] = ilutGLLoadImage(texName);
	}
}

/******************************************************************
  Function:   loadHeightMap
  Purpose:    load height map from 3ds object
******************************************************************/
void RaceTrack::loadHeightMap()
{
	int numVertices = 0;
	std::vector<btVector3> points;

	// Loop through all meshes that are apart of the file.
	for(int i = 0; i < track3DModel.totalMeshes; i++)
	{
		// look for mesh with name "ground"
		if (memcmp(track3DModel.meshList[i].name, "ground", 6) == 0)
		{
			stMesh *pMesh = &track3DModel.meshList[i];
			numVertices = pMesh->totalVertices;

			points.resize(numVertices);
			rawHeightfieldData = (float *)malloc(numVertices * sizeof(float));

			// temporarily store mesh vertices in vector
			for (int j = 0; j < numVertices; ++j)
			{
				points[j].setX(pMesh->pVertices[j].x);
				points[j].setY(pMesh->pVertices[j].y);
				points[j].setZ(pMesh->pVertices[j].z);
			}

			// Because the heights from the 3ds file are not in
			// order we must order it ourselves.
			std::sort(points.begin(), points.end(), Ordering_Functor());
			// and finally set the height values
			for(int j = 0; j < numVertices; ++j)
			{
				rawHeightfieldData[j] = points[j].z();
				//printf("height: %f\n", rawHeightfieldData[j]);
			}
			continue;
		}
	}
}

/******************************************************************
  Function:   createDisplayList
  Purpose:    generate display list since object is static
******************************************************************/
void RaceTrack::createDisplayList()
{
	mapDisplayListID = glGenLists(1);

	glNewList(mapDisplayListID,GL_COMPILE);
	glPushMatrix();
		glRotatef(-90,1,0,0);
		glRotatef(-90,0,0,1);

		// Loop through all meshes that are apart of the file and render them.
		//glEnable(GL_TEXTURE_2D);
		for(int i = 0; i < track3DModel.totalMeshes; i++)
		{
			char * tName = track3DModel.materialList[track3DModel.meshList[i].pFaces[0].matId].textureName;
			if (numTextures > 0 && strlen(tName))
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texNamesMap[std::string(tName)]);
			}

			glBegin(GL_TRIANGLES);
			// each mesh is composed of several triangular faces. Draw them
			for(unsigned int f = 0; f < track3DModel.meshList[i].totalFaces; f++)
			{
				// Get pointers to make the below code cleaner.
				// meshList contains meshes and meshes contain vertices and faces
				// they also contain texture coordinates
				stMesh *pMesh = &track3DModel.meshList[i];
				stFace *pFace = pMesh->pFaces;
				stTexCoord *pTexCoord = pMesh->pTexCoords;
				unsigned char colorR  = track3DModel.materialList[pFace[f].matId].color.r;
				unsigned char colorG  = track3DModel.materialList[pFace[f].matId].color.g;
				unsigned char colorB  = track3DModel.materialList[pFace[f].matId].color.b;

				// If one or both or NULL, we got a problem.
				if(!pMesh || !pFace) continue;

				// Draw the triangle.
				glNormal3f(pFace[f].normal.x, pFace[f].normal.y, pFace[f].normal.z);

				glColor3ub(colorR, colorG, colorB);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[0]].tu, pTexCoord[pFace[f].indices[0]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[0]].x,
                       pMesh->pVertices[pFace[f].indices[0]].y,
                       pMesh->pVertices[pFace[f].indices[0]].z);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[1]].tu, pTexCoord[pFace[f].indices[1]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[1]].x,
                       pMesh->pVertices[pFace[f].indices[1]].y,
                       pMesh->pVertices[pFace[f].indices[1]].z);

				if(pTexCoord) glTexCoord2f(pTexCoord[pFace[f].indices[2]].tu, pTexCoord[pFace[f].indices[2]].tv);
				glVertex3f(pMesh->pVertices[pFace[f].indices[2]].x,
                       pMesh->pVertices[pFace[f].indices[2]].y,
                       pMesh->pVertices[pFace[f].indices[2]].z);
			}
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
      //glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEndList();
}

/******************************************************************
  Function:   cleanUpRaceTrack
  Purpose:    delete race track data
******************************************************************/
void RaceTrack::cleanUpRaceTrack()
{
   //free(rawHeightfieldData);
	rawHeightfieldData = NULL;
}

/******************************************************************
  Function:   renderRaceTrack
  Purpose:    render 3ds race track
******************************************************************/
void RaceTrack::renderRaceTrack()
{
	glCallList(mapDisplayListID);
}
