/*
   File:

      3dsObject.cpp

   Created by:

      Allen Sherrod (Programming Ace of www.UltimateGameProgramming.com).

   Description:

      This class represents a 3D model loaded from a 3DS object file.
*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include "3dsObject.h"
#include "3dsChunks.h"

C3DSObject::C3DSObject()
{
	totalMeshes = 0;
	totalMaterials = 0;
}

bool C3DSObject::LoadModel(char *fileName)
{
   FILE *fp = NULL;
   stChunk primaryChunk;

   if(!fileName) return false;

   fp = fopen(fileName, "rb");
   if(!fp) return false;

   // The first chunk in the file is called the primary chunk.
   ReadChunk(fp, &primaryChunk);

   // This test will tell us if this is a 3DS file.
   if(primaryChunk.id != _3DS_PRIMARY_ID) return false;

   // Recursively read the model file.
   ReadSubChunk(fp, &primaryChunk);
   fclose(fp);

   stVertex v1, v2;
   stVertex n;
   // This is used to create triangle normals.  If you want your
   // 3DS loader to support bump mapping, adding s and t tangents
   // below will do that for you.
   for(int i = 0; i < totalMeshes; i++)
   {
      for(unsigned int f = 0; f < meshList[i].totalFaces; f++)
      {
         stMesh *pMesh = &meshList[i];
         stFace *pFace = pMesh->pFaces;

         if(!pMesh || !pFace) continue;

         v1.x = pMesh->pVertices[pFace[f].indices[0]].x - pMesh->pVertices[pFace[f].indices[1]].x;
         v1.y = pMesh->pVertices[pFace[f].indices[0]].y - pMesh->pVertices[pFace[f].indices[1]].y;
         v1.z = pMesh->pVertices[pFace[f].indices[0]].z - pMesh->pVertices[pFace[f].indices[1]].z;

         v2.x = pMesh->pVertices[pFace[f].indices[1]].x - pMesh->pVertices[pFace[f].indices[2]].x;
         v2.y = pMesh->pVertices[pFace[f].indices[1]].y - pMesh->pVertices[pFace[f].indices[2]].y;
         v2.z = pMesh->pVertices[pFace[f].indices[1]].z - pMesh->pVertices[pFace[f].indices[2]].z;

         n.x = ((v1.y * v2.z) - (v1.z * v2.y));
         n.y = ((v1.z * v2.x) - (v1.x * v2.z));
         n.z = ((v1.x * v2.y) - (v1.y * v2.x));

         float len = 1 / (float)sqrt((n.x * n.x + n.y * n.y + n.z * n.z));
         n.x *= len; n.y *= len; n.z *= len;

         pFace[f].normal.x = n.x;
         pFace[f].normal.y = n.y;
         pFace[f].normal.z = n.z;
      }
   }

   return true;
}


void C3DSObject::ReadSubChunk(FILE *fp, stChunk *pChunk)
{
   stMesh mesh;
   stMesh *pMesh = NULL;
   stMaterial *pMat = NULL;
   stMaterial mat;
   char name[256];

   // Keep reading until we have finished reading this chunk.
   while(pChunk->bytesRead < pChunk->length)
   {
      // Read the next chunk.
      stChunk chunk;
      ReadChunk(fp, &chunk);

      // Depending on the chunk will depend on what
      // is being loaded.
      /*if(chunk.id == _3DS_EDIT3DS)
      {
         // Read the next chunk to get the actual data.
         ReadSubChunk(fp, &chunk);
      }
      else if(chunk.id == _3DS_NAMED_OBJECT)
      {
         // Add a new mesh object to the list.
         meshList.push_back(mesh);
         totalMeshes++;

         // Load mesh data.
         chunk.bytesRead += GetNextString(fp, name);
         pMesh = &(meshList[totalMeshes - 1]);
         strcpy(pMesh->name, name);

         ReadSubChunk(fp, &chunk);
      }
      else if(chunk.id == _3DS_OBJ_MESH)
      {
         // Read the next chunk to get the actual data.
         ReadSubChunk(fp, &chunk);
      }
      else if(chunk.id == _3DS_MESH_VERTICES)
      {
         // Read vertex points.
         ReadVertices(fp, &chunk);
      }
      else if(chunk.id == _3DS_MESH_FACES)
      {
         // Read triangles.
         ReadFaces(fp, &chunk);
      }
      else if(chunk.id == _3DS_MESH_MATER)
      {
         // Read material indices.
         ReadMeshMaterials(fp, &chunk);
      }
      else if(chunk.id == _3DS_MESH_TEX_VERT)
      {
         // Read texture coords.
         ReadTexCoords(fp, &chunk);
      }
      else if(chunk.id == _3DS_MATERIAL)
      {
         // Add a new material to the list then read it.
         materialList.push_back(mat);
         totalMaterials++;

         ReadSubChunk(fp, &chunk);
      }
      else if(chunk.id == _3DS_MAT_NAME)
      {
         // Read materials name and save it.
         chunk.bytesRead += GetNextString(fp, name);
         pMat = &(materialList[totalMaterials - 1]);
         strcpy(pMat->name, name);
      }
      else if(chunk.id == _3DS_MAT_DIFFUSE)
      {
         // Read diffuse colors.
         ReadDiffuse(fp, &chunk);
      }
      else if(chunk.id == _3DS_MAT_TEXMAP)
      {
         // Read the next chunk to get the actual data.
         ReadSubChunk(fp, &chunk);
      }
      else if(chunk.id == _3DS_MAT_TEXFLNM)
      {
         // Read texture filename and save it.
         chunk.bytesRead += GetNextString(fp, name);
         pMat = &(materialList[totalMaterials - 1]);
         strcpy(pMat->textureName, name);
      }
      else
      {
         // Skip chunk cuz we don't support
         // whatever is here.
         MoveToNextChunk(fp, &chunk);
      }*/

      switch(chunk.id)
      {
         case _3DS_EDIT3DS:
            // Read the next chunk to get the actual data.
            ReadSubChunk(fp, &chunk);
            break;

         case _3DS_NAMED_OBJECT:
            // Add a new mesh object to the list.
            meshList.push_back(mesh);
            totalMeshes++;

            // Load mesh data.
            chunk.bytesRead += GetNextString(fp, name);
            pMesh = &(meshList[totalMeshes - 1]);
            strcpy(pMesh->name, name);

            ReadSubChunk(fp, &chunk);
            break;

         case _3DS_OBJ_MESH:
            // Read the next chunk to get the actual data.
            ReadSubChunk(fp, &chunk);
            break;

         case _3DS_MESH_VERTICES:
            // Read vertex points.
            ReadVertices(fp, &chunk);
            break;

         case _3DS_MESH_FACES:
            // Read triangles.
            ReadFaces(fp, &chunk);
            break;

         case _3DS_MESH_MATER:
            // Read material indices.
            ReadMeshMaterials(fp, &chunk);
            break;

         case _3DS_MESH_TEX_VERT:
            // Read texture coords.
            ReadTexCoords(fp, &chunk);
            break;

         case _3DS_MATERIAL:
            // Add a new material to the list then read it.
            materialList.push_back(mat);
            totalMaterials++;

            ReadSubChunk(fp, &chunk);
            break;

         case _3DS_MAT_NAME:
            // Read materials name and save it.
            chunk.bytesRead += GetNextString(fp, name);
            pMat = &(materialList[totalMaterials - 1]);
            strcpy(pMat->name, name);
            break;

         case _3DS_MAT_DIFFUSE:
            // Read diffuse colors.
            ReadDiffuse(fp, &chunk);
            break;

         case _3DS_MAT_TEXMAP:
            // Read the next chunk to get the actual data.
            ReadSubChunk(fp, &chunk);
            break;

         case _3DS_MAT_TEXFLNM:
            // Read texture filename and save it.
            chunk.bytesRead += GetNextString(fp, name);
            pMat = &(materialList[totalMaterials - 1]);
            strcpy(pMat->textureName, name);
            break;

         default:
            // Skip chunk cuz we don't support
            // whatever is here.
            MoveToNextChunk(fp, &chunk);
            break;
      }

      // Add to the total bytes loaded.
      pChunk->bytesRead += chunk.length;
   }
}


void C3DSObject::MoveToNextChunk(FILE *fp, stChunk *pChunk)
{
   int buff[50000];
   // Skip a chunks data.
   fread(buff, 1, pChunk->length - pChunk->bytesRead, fp);
}


int C3DSObject::GetNextString(FILE *fp, char *str)
{
   char buff[100] = { 0 };
   int index = 0;

   // Read an entire string.
   fread(buff, 1, 1, fp);
   while(*(buff + index) != 0)
   {
      index++;
      fread(buff + index, 1, 1, fp);
   }

   // Save the string and return the size of the string.
   strcpy(str, buff);
   return strlen(buff) + 1;
}


void C3DSObject::ReadChunk(FILE *fp, stChunk *pChunk)
{
   if(!pChunk) return;

   // Read chunk header.
   pChunk->bytesRead = fread(&pChunk->id, 1, 2, fp);
   pChunk->bytesRead += fread(&pChunk->length, 1, 4, fp);
}


void C3DSObject::ReadDiffuse(FILE *fp, stChunk *pChunk)
{
   stColor color;
   char header[6];

   // Read a color value.
   pChunk->bytesRead += fread(header, 1, 6, fp);
   pChunk->bytesRead += fread(&color, 1, 3, fp);

   // Save this color.
   stMaterial *mat = &(materialList[totalMaterials - 1]);
   mat->color.r = color.r;
   mat->color.g = color.g;
   mat->color.b = color.b;

   // Skip the rest of the chunk.
   MoveToNextChunk(fp, pChunk);
}


void C3DSObject::ReadMeshMaterials(FILE *fp, stChunk *pChunk)
{
   char name[256];
   int matId = 0;
   unsigned short totalMatIndices = 0;
   unsigned short *pMatIndices = NULL;

   // Read material name and total.
   pChunk->bytesRead += GetNextString(fp, name);
   pChunk->bytesRead += fread(&totalMatIndices, 1, 2, fp);

   // Allocate the material indices.
   pMatIndices = new unsigned short[totalMatIndices];

   // Read indices.
   pChunk->bytesRead += fread(pMatIndices, 1, totalMatIndices *
                              sizeof(unsigned short), fp);

   // Look if this material already exist in the list.
   int i =0;
   for(i = 0; i < totalMaterials; i++)
   {
      if(strcmp(name, materialList[i].name) == 0)
         matId = i;
   }

   // Get the current mesh.
   stMesh *pMesh = &(meshList[totalMeshes - 1]);

   // Add the material id to all faces that uses it.
   for(i = 0; i < totalMatIndices; i++)
      pMesh->pFaces[pMatIndices[i]].matId = matId;
}


void C3DSObject::ReadFaces(FILE *fp, stChunk *pChunk)
{
   unsigned int totalFaces = 0;
   stFileFace *pFaces;

   // Read number of faces.
   pChunk->bytesRead += fread(&totalFaces, 1, 2, fp);

   // Read faces.
   pFaces = new stFileFace[totalFaces];
   pChunk->bytesRead += fread(pFaces, 1, totalFaces *
                              sizeof(stFileFace), fp);

   // Get current mesh and a pointer to its faces.
   stMesh *pMesh = &(meshList[totalMeshes - 1]);
   pMesh->pFaces = new stFace[totalFaces];
   pMesh->totalFaces = totalFaces;

   // Loop through and copy the face data.
   for(unsigned int i = 0; i < totalFaces; i++)
   {
      pMesh->pFaces[i].indices[0] = pFaces[i].indices[0];
      pMesh->pFaces[i].indices[1] = pFaces[i].indices[1];
      pMesh->pFaces[i].indices[2] = pFaces[i].indices[2];
   }

   // Delete temp memory.
   delete[] pFaces;

   // Read the next chunk.
   ReadSubChunk(fp, pChunk);
}


void C3DSObject::ReadVertices(FILE *fp, stChunk *pChunk)
{
   unsigned int totalVertices = 0;
   stVertex *pVertices;
   // Read number of vertices.
   pChunk->bytesRead += fread(&totalVertices, 1, 2, fp);

   // Load vertex points.
   pVertices = new stVertex[totalVertices];
   pChunk->bytesRead += fread(pVertices, 1, totalVertices *
                              sizeof(stVertex), fp);
   // Get the current mesh and save the data to it.
   stMesh *pMesh = &(meshList[totalMeshes - 1]);
   pMesh->pVertices = pVertices;
   pMesh->totalVertices = totalVertices;
   // Skip any data left in this chunk.
   MoveToNextChunk(fp, pChunk);
}


void C3DSObject::ReadTexCoords(FILE *fp, stChunk *pChunk)
{
   unsigned int totalTexCoords = 0;
   stTexCoord *pTexCoords;

   // Read total.
   pChunk->bytesRead += fread(&totalTexCoords, 1, 2, fp);

   // Read all the tex coords.
   pTexCoords = new stTexCoord[totalTexCoords];
   pChunk->bytesRead += fread(pTexCoords, 1, totalTexCoords *
                              sizeof(stTexCoord), fp);

   // Get the current mesh and save the data to it.
   stMesh *pMesh = &(meshList[totalMeshes - 1]);
   pMesh->pTexCoords = pTexCoords;

   // Skip anything that is left.
   MoveToNextChunk(fp, pChunk);
}


void C3DSObject::Shutdown()
{
   // Loop through and make sure all data is released.
   for(int i = 0; i < totalMeshes; i++)
   {
      if(meshList[i].pFaces)
      {
         delete[] meshList[i].pFaces;
         meshList[i].pFaces = NULL;
      }

      if(meshList[i].pVertices)
      {
         delete[] meshList[i].pVertices;
         meshList[i].pVertices = NULL;
      }

      if(meshList[i].pTexCoords)
      {
         delete[] meshList[i].pTexCoords;
         meshList[i].pTexCoords = NULL;
      }

      meshList[i].totalFaces = 0;
      meshList[i].totalVertices = 0;
      meshList[i].totalTexCoords = 0;
   }

   // Free the list.
   for(std::vector<stMesh>::iterator itMesh = meshList.begin(); itMesh != meshList.end();)
   {
      meshList.erase(itMesh);
   }

   for(std::vector<stMaterial>::iterator itMat = materialList.begin(); itMat != materialList.end();)
   {
      materialList.erase(itMat);
   }

   totalMeshes = 0;
   totalMaterials = 0;
}


// Copyright Februsry 2005
// All Rights Reserved!
// Allen Sherrod
// ProgrammingAce@UltimateGameProgramming.com
// www.UltimateGameProgramming.com
