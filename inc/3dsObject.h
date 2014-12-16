/*
   File:

      3dsObject.h

   Created by:

      Allen Sherrod (Programming Ace of www.UltimateGameProgramming.com).

   Description:

      This class represents a 3D model loaded from a 3DS object file.
*/


#ifndef _3DS_OBJECT_H_
#define _3DS_OBJECT_H_

#include <stdio.h>
#include <vector>


// File chunk header.
struct stChunk
{
   stChunk() : id(0), length(0), bytesRead(0) {}
   unsigned short id;
   unsigned int length;
   unsigned int bytesRead;
};

// Vertex position.
struct stVertex
{
   stVertex() : x(0), y(0), z(0) {}
   float x, y, z;
};

// Triangle.
struct stFace
{
   stFace() : matId(0) { indices[0] = indices[1] = indices[2] = 0; }
   unsigned int indices[3];
   stVertex normal;
   int matId;
};

// Triangle as it is in the file.
struct stFileFace
{
   stFileFace() : vis(0) { indices[0] = indices[1] = indices[2] = 0; }
   unsigned short indices[3], vis;
};

// RGB color value.
struct stColor
{
   stColor() : r(0), g(0), b(0), a(0) {}
   unsigned char r, g, b, a;
};

// Texture coordinate for a vertex pos.
struct stTexCoord
{
   stTexCoord() : tu(0), tv(0) {}
   float tu, tv;
};

// Material of a face.
struct stMaterial
{
   stMaterial() { name[0] = '\0'; textureName[0] = '\0'; }
   char name[256];
   stColor color;
   char textureName[256];
};

// Mesh object.  A file can have more than 1.
struct stMesh
{
   stMesh() : pFaces(0), pVertices(0), pTexCoords(0),
              totalFaces(0), totalVertices(0), totalTexCoords(0) {}

   char name[256];

   stFace *pFaces;
   stVertex *pVertices;
   stTexCoord *pTexCoords;

   unsigned int totalFaces;
   unsigned int totalVertices;
   unsigned int totalTexCoords;
};

// 3DS Loader.
class C3DSObject
{
public:
   C3DSObject();
   ~C3DSObject() { Shutdown(); }

   // Load the entire file.
   bool LoadModel(char *fileName);

   // Deals with reading chunk headers.
   void ReadSubChunk(FILE *fp, stChunk *pChunk);
   void MoveToNextChunk(FILE *fp, stChunk *pChunk);
   int GetNextString(FILE *fp, char *str);
   void ReadChunk(FILE *fp, stChunk *pChunk);

   // Read different sections out of the file like vertex
   // positions, tex coords, faces, etc.
   void ReadMaterials(FILE *fp, stChunk *pChunk);
   void ReadDiffuse(FILE *fp, stChunk *pChunk);
   void ReadMeshMaterials(FILE *fp, stChunk *pChunk);
   void ReadFaces(FILE *fp, stChunk *pChunk);
   void ReadVertices(FILE *fp, stChunk *pChunk);
   void ReadTexCoords(FILE *fp, stChunk *pChunk);

   void Shutdown();

   // List of mesh objects if there are more than 1,
   // list of materials, and counters for each.
   std::vector<stMesh> meshList;
   std::vector<stMaterial> materialList;
   int totalMeshes;
   int totalMaterials;
};

#endif


// Copyright February 2005
// All Rights Reserved!
// Allen Sherrod
// ProgrammingAce@UltimateGameProgramming.com
// www.UltimateGameProgramming.com
