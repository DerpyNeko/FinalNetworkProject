#include "globalOpenGLStuff.h"
#include "globalStuff.h"

// Loading models was moved into this function
void LoadModelTypes(cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID)
{
	sModelDrawInfo terrainInfo;
	terrainInfo.meshFileName = "Terrain.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(terrainInfo, shaderProgramID);

	sModelDrawInfo sphereInfo;
	sphereInfo.meshFileName = "Sphere_n_uv.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInfo, shaderProgramID);

	sModelDrawInfo cubeInfo;
	cubeInfo.meshFileName = "cube_n_uv.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(cubeInfo, shaderProgramID);

	sModelDrawInfo sphereInvertedNormalsInfo;
	sphereInvertedNormalsInfo.meshFileName = "Sphere_n_uv_INVERTED_NORMALS.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInvertedNormalsInfo, shaderProgramID);

	// At this point, mesh in in GPU
	std::cout << "Mesh was loaded OK" << std::endl;

	// Load the textures, too
	::g_pTextureManager->SetBasePath("assets/textures");
	::g_pTextureManager->Create2DTextureFromBMPFile("SandTexture.bmp", true);

	// Load the cube map
	::g_pTextureManager->SetBasePath("assets/textures/cubemaps");
	std::string errorString;
	if (::g_pTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap",
		"TropicalSunnyDayLeft2048.bmp", "TropicalSunnyDayRight2048.bmp",		// Alternate these
		"TropicalSunnyDayDown2048.bmp", "TropicalSunnyDayUp2048.bmp", 			// Rotate these 90 degrees
		"TropicalSunnyDayFront2048.bmp", "TropicalSunnyDayBack2048.bmp", true, errorString))
	{
		std::cout << "Loaded the city cube map OK" << std::endl;
	}
	else
	{
		std::cout << "Error: city cube map DIDN't load. On no!" << std::endl;
	}

	return;
}

// Loads the models we are drawing into the vector
void LoadModelsIntoScene(std::vector<cMeshObject*> &vec_pObjectsToDraw)
{
	// skybox
	{
		// (could also be a cube, or whatever)
		cMeshObject* pSkyBoxObject = new cMeshObject();
		pSkyBoxObject->setDiffuseColour(glm::vec3(1.0f, 105.0f / 255.0f, 180.0f / 255.0f));
		pSkyBoxObject->bUseVertexColour = false;
		pSkyBoxObject->friendlyName = "SkyBoxObject";
		float scale = 5000.0f;
		pSkyBoxObject->nonUniformScale = glm::vec3(scale, scale, scale);
		pSkyBoxObject->meshName = "Sphere_n_uv_INVERTED_NORMALS.ply";
		// Invisible until I need to draw it
		pSkyBoxObject->bIsVisible = false;

		vec_pObjectsToDraw.push_back(pSkyBoxObject);
	}

	// cube
	{
		cMeshObject* pCube = new cMeshObject();

		//pCube->position = glm::vec3(1000.0f, 0.0f, 0.0f);
		pCube->setDiffuseColour(glm::vec3(0.1f, 0.1f, 0.1f));
		pCube->setUniformScale(1.0f);
		pCube->setSpecularPower(100.0f);

		pCube->friendlyName = "Cube";
		pCube->meshName = "cube_n_uv.ply";
		pCube->bIsVisible = false;

		//sTextureInfo sandTexture;
		//sandTexture.name = "SandTexture.bmp";
		//sandTexture.strength = 1.0f;
		//pCube->vecTextures.push_back(sandTexture);

		pCube->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pCube);
	}

	{	// This sphere is the tiny little debug sphere
		cMeshObject* pDebugSphere = new cMeshObject();
		pDebugSphere->meshName = "Sphere_n_uv.ply";
		pDebugSphere->friendlyName = "DebugSphere";
		pDebugSphere->position = glm::vec3(0.0f, 0.0f, 0.0f);
		pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
		pDebugSphere->setUniformScale(0.1f);

		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = false;

		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
	}

	return;
}