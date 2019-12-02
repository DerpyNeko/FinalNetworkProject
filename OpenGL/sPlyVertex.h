#ifndef _sPlyVertex_HG_
#define _sPlyVertex_HG_

struct sPlyVertex 
{
	float x;
	float y;
	float z;
	
	// Now has normals (this is FROM THE FILE)
	float nx;
	float ny;
	float nz;
	
	// Load a single UV (texture coordinate)
	float u;
	float v;
};
#endif