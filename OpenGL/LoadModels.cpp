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

	{
		cMeshObject* pTerrain = new cMeshObject();

		pTerrain->position = glm::vec3(0.0f, 0.0f, 0.0f);
		pTerrain->setDiffuseColour(glm::vec3(0.1f, 0.1f, 0.1f));
		pTerrain->setUniformScale(1.0f);
		pTerrain->setSpecularPower(100.0f);

		pTerrain->friendlyName = "Terrain";
		pTerrain->meshName = "Terrain.ply";
		pTerrain->bIsVisible = true;

		sTextureInfo sandTexture;
		sandTexture.name = "SandTexture.bmp";
		sandTexture.strength = 1.0f;
		pTerrain->vecTextures.push_back(sandTexture);

		pTerrain->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pTerrain);
	}

	{
		cMeshObject* pDebugSphere = new cMeshObject();
		pDebugSphere->meshName = "Sphere_n_uv.ply";
		pDebugSphere->friendlyName = "StartA";
		pDebugSphere->position = glm::vec3(-145.0f, 20.0f, -210.0f);
		pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
		pDebugSphere->setUniformScale(5.0f);

		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = true;
		pDebugSphere->bIsUpdatedByPhysics = true;

		pDebugSphere->pTheShape = new sSphere(5.0f);
		pDebugSphere->shapeType = cMeshObject::SPHERE;

		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
	}

	{
		cMeshObject* pDebugSphere = new cMeshObject();
		pDebugSphere->meshName = "Sphere_n_uv.ply";
		pDebugSphere->friendlyName = "StartT";
		pDebugSphere->position = glm::vec3(-145.0f, 20.0f, -210.0f);
		pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 1.0f, 0.0f));
		pDebugSphere->setUniformScale(5.0f);

		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = false;
		pDebugSphere->bIsUpdatedByPhysics = true;

		pDebugSphere->pTheShape = new sSphere(5.0f);
		pDebugSphere->shapeType = cMeshObject::SPHERE;

		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
	}

	{
		cMeshObject* pDebugSphere = new cMeshObject();
		pDebugSphere->meshName = "Sphere_n_uv.ply";
		pDebugSphere->friendlyName = "EndA";
		pDebugSphere->position = glm::vec3(-90.0f, 20.0f, -210.0f);
		pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
		pDebugSphere->setUniformScale(5.0f);

		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = true;
		pDebugSphere->bIsUpdatedByPhysics = true;

		pDebugSphere->pTheShape = new sSphere(5.0f);
		pDebugSphere->shapeType = cMeshObject::SPHERE;

		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
	}

	{
		cMeshObject* pDebugSphere = new cMeshObject();
		pDebugSphere->meshName = "Sphere_n_uv.ply";
		pDebugSphere->friendlyName = "EndT";
		pDebugSphere->position = glm::vec3(-90.0f, 20.0f, -210.0f);
		pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 1.0f));
		pDebugSphere->setUniformScale(5.0f);

		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = false;
		pDebugSphere->bIsUpdatedByPhysics = true;

		pDebugSphere->pTheShape = new sSphere(5.0f);
		pDebugSphere->shapeType = cMeshObject::SPHERE;

		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
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