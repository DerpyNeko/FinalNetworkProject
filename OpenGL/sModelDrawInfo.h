#ifndef _sModelDrawInfo_HG_
#define _sModelDrawInfo_HG_

#include <string>
#include "sPlyVertex.h"
#include "sPlyTriangle.h"

struct sVert_xyz_rgb_Nxyz_uv2
{
	sVert_xyz_rgb_Nxyz_uv2() :
		x(0.0f), y(0.0f), z(0.0f),
		r(0.0f), g(0.0f), b(0.0f),
		nx(0.0f), ny(0.0f), nz(0.0f),
		u0(0.0f), v0(0.0f), u1(0.0f), v1(0.0f) {};
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
	float u0, v0, u1, v1;
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshFileName;

	unsigned int VAO_ID;

	unsigned int vertexBufferID;
	unsigned int vertexBuffer_start_index;
	unsigned int numberOfVertices;

	unsigned int indexBufferID;
	unsigned int indexBuffer_start_index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// This is the vertex information as read from the file
	sPlyVertex* pVerticesFromFile;

	// These are the original triangle values from file, but in the 'triangle' (or face) format
	sPlyTriangle* pTriangles;

	// This is the vertex information being passed to the GPU (is in format that the shader needs)
	sVert_xyz_rgb_Nxyz_uv2* pVerticesToVBO;

	// This buffer is indices as a 1D array (that the GPU needs)
	unsigned int* pIndices;

	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	float extentX, extentY, extentZ;
	float maxExtent;

	// ******************************************
	// This is to draw "debug lines" in the scene
	// HACK: Can make this dynamic
	bool bVertexBufferIsDynamic;	// = false by default
	bool bIsIndexedModel;			// = true by default
};

#endif 
