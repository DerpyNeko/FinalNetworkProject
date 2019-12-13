#include "globalOpenGLStuff.h"
#include "globalStuff.h"

// Loading models was moved into this function
void LoadModelTypes(cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID)
{
	sModelDrawInfo sphereInfo;
	sphereInfo.meshFileName = "Sphere_n_uv.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInfo, shaderProgramID);

	sModelDrawInfo cubeInfo;
	cubeInfo.meshFileName = "cube_n_uv.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(cubeInfo, shaderProgramID);

	sModelDrawInfo playerInfo;
	playerInfo.meshFileName = "player.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(playerInfo, shaderProgramID);

	sModelDrawInfo shotInfo;
	shotInfo.meshFileName = "hitsphere.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(shotInfo, shaderProgramID);

	sModelDrawInfo sphereInvertedNormalsInfo;
	sphereInvertedNormalsInfo.meshFileName = "Sphere_n_uv_INVERTED_NORMALS.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInvertedNormalsInfo, shaderProgramID);

	// At this point, mesh in in GPU
	std::cout << "Mesh was loaded OK" << std::endl;

	// Load the textures, too
	::g_pTextureManager->SetBasePath("assets/textures");
	::g_pTextureManager->Create2DTextureFromBMPFile("SandTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("DarkCube.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("LightCube.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GreyCube.bmp", true);


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
void LoadModelsIntoScene(std::vector<cMeshObject*>& vec_pObjectsToDraw)
{
	// pPlayer object
	{
		for (int x = 0; x < 11; x = x + 10)
		{
			for (int z = 0; z < 11; z = z + 10)
			{
				cMeshObject* pPlayer = new cMeshObject();
				pPlayer->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
				pPlayer->setUniformScale(0.15f);
				pPlayer->setSpecularPower(100.0f);
				pPlayer->position = glm::vec3((float)x, 1.0f, (float)z);
				std::cout << "Starting Position: " << pPlayer->position.x << ", " << pPlayer->position.y << ", " << pPlayer->position.z << std::endl;

				pPlayer->meshName = "player.ply";
				pPlayer->bIsVisible = true;

				vec_pObjectsToDraw.push_back(pPlayer);
				g_Players.push_back(pPlayer);

				cMeshObject* pBullet = new cMeshObject();

				pBullet->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
				pBullet->setSpecularPower(100.0f);
				pBullet->position = glm::vec3((float)x, 1.0f, (float)z);
				pBullet->setUniformScale(0.1f);
				pBullet->meshName = "hitsphere.ply";
				pBullet->bIsVisible = true;

				pBullet->pDebugRenderer = ::g_pDebugRenderer;
				vec_pObjectsToDraw.push_back(pBullet);
				g_Bullets.push_back(pBullet);	
				
				//cMeshObject* pHitBox = new cMeshObject();

				//pHitBox->setDiffuseColour(glm::vec3(0.0f, 0.0f, 1.0f));
				//pHitBox->setSpecularPower(100.0f);
				//pHitBox->position = glm::vec3((float)x, 1.0f, (float)z);
				//pHitBox->setUniformScale(0.15f);
				//pHitBox->meshName = "hitsphere.ply";
				//pHitBox->bIsVisible = true;

				//pHitBox->pDebugRenderer = ::g_pDebugRenderer;
				//vec_pObjectsToDraw.push_back(pHitBox);
				//g_HitBoxes.push_back(pHitBox);
			}
		}
	}

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

	// cube - light texture
	{
		cMeshObject* pCube = new cMeshObject();

		pCube->setDiffuseColour(glm::vec3(0.1f, 0.1f, 0.1f));
		pCube->setUniformScale(1.0f);
		pCube->setSpecularPower(100.0f);

		pCube->friendlyName = "CubeLight";
		pCube->meshName = "cube_n_uv.ply";
		pCube->bIsVisible = false;

		//sTextureInfo lightTexture;
		//lightTexture.name = "LightCube.bmp";
		//lightTexture.strength = 1.0f;
		//pCube->vecTextures.push_back(lightTexture);

		pCube->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pCube);
	}

	// cube - dark texture
	{
		cMeshObject* pCube = new cMeshObject();

		pCube->setDiffuseColour(glm::vec3(0.1f, 0.1f, 0.1f));
		pCube->setUniformScale(1.0f);
		pCube->setSpecularPower(100.0f);

		pCube->friendlyName = "CubeDark";
		pCube->meshName = "cube_n_uv.ply";
		pCube->bIsVisible = false;

		sTextureInfo darkTexture;
		darkTexture.name = "GreyCube.bmp";
		darkTexture.strength = 1.0f;
		pCube->vecTextures.push_back(darkTexture);

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