//     ___                 ___ _     
//    / _ \ _ __  ___ _ _ / __| |    
//   | (_) | '_ \/ -_) ' \ (_ | |__  
//    \___/| .__/\___|_||_\___|____| 
//         |_|                       
//

#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

#include "cShaderManager.h"
#include "DebugRenderer/cDebugRenderer.h"
#include "cLightHelper.h"

#include "cAABB.h"

cDebugRenderer* g_pDebugRendererACTUAL = NULL;
iDebugRenderer* g_pDebugRenderer = NULL;

cShaderManager* pShaderManager = NULL;		// "Heap" variable
cVAOMeshManager* g_pVAOMeshManager = NULL;
cBasicTextureManager* g_pTextureManager = NULL;

cCamera* g_pCamera = NULL;
std::vector<cMeshObject*> g_vec_pObjectsToDraw;
std::vector<sLight*> g_Lights;

int g_ModelIndex = 0;
bool g_bDrawDebugLightSpheres = false;
void DrawDebugLightSpheres(cLightHelper* pLightHelper, sLight* light, cMeshObject* pDebugSphere, glm::mat4 matBall, GLuint program, glm::vec4 oldDiffuse, glm::vec3 oldScale);
void UpdateWindowTitle(GLFWwindow* window);
void BubbleSort(std::vector<cMeshObject*>& vec);

cAABBHierarchy* g_pTerrain = new cAABBHierarchy();
void LoadTerrainAABB(void);

void CheckCollision(cMeshObject* target, cMeshObject* actual, std::vector< cAABB::sAABB_Triangle > triangles);

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1280, 720, "Checkpoint 6", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	// Mouse callbacks
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Create the shader manager...
	pShaderManager = new cShaderManager();
	pShaderManager->setBasePath("assets/shaders/");

	cShaderManager::cShader vertexShader;
	cShaderManager::cShader fragmentShader;

	vertexShader.fileName = "vertex01.glsl";
	vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

	fragmentShader.fileName = "fragment01.glsl";
	fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

	if (pShaderManager->createProgramFromFile("BasicUberShader", vertexShader, fragmentShader))
	{
		std::cout << "Compiled shaders OK." << std::endl;
	}
	else
	{
		std::cout << "OH NO! Compile error" << std::endl;
		std::cout << pShaderManager->getLastError() << std::endl;
	}

	// Load the uniform location values (some of them, anyway)
	cShaderManager::cShaderProgram* pSP = ::pShaderManager->pGetShaderProgramFromFriendlyName("BasicUberShader");
	pSP->LoadUniformLocation("texture00");
	pSP->LoadUniformLocation("texture01");
	pSP->LoadUniformLocation("texture02");
	pSP->LoadUniformLocation("texture03");
	pSP->LoadUniformLocation("texture04");
	pSP->LoadUniformLocation("texture05");
	pSP->LoadUniformLocation("texture06");
	pSP->LoadUniformLocation("texture07");
	pSP->LoadUniformLocation("texBlendWeights[0]");
	pSP->LoadUniformLocation("texBlendWeights[1]");

	GLuint program = pShaderManager->getIDFromFriendlyName("BasicUberShader");

	::g_pVAOMeshManager = new cVAOMeshManager();
	::g_pTextureManager = new cBasicTextureManager();

	// Loading the uniform variables here (rather than the inner draw loop)
	GLint objectColour_UniLoc = glGetUniformLocation(program, "objectColour");

	GLint matModel_location = glGetUniformLocation(program, "matModel");
	GLint matView_location = glGetUniformLocation(program, "matView");
	GLint matProj_location = glGetUniformLocation(program, "matProj");
	GLint eyeLocation_location = glGetUniformLocation(program, "eyeLocation");

	// Note that this point is to the +interface+ but we're creating the actual object
	::g_pDebugRendererACTUAL = new cDebugRenderer();
	::g_pDebugRenderer = (iDebugRenderer*)::g_pDebugRendererACTUAL;

	if (!::g_pDebugRendererACTUAL->initialize())
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRendererACTUAL->getLastError() << std::endl;
	}
	else
	{
		std::cout << "Debug renderer is OK" << std::endl;
	}

	// Camera creation
	::g_pCamera = new cCamera();
	g_pCamera->eye = { -890.0f, 115.0f, -810.0f };
	g_pCamera->setCameraAt(glm::vec3(0.785847f, 0.00212923f, 0.61842f));

	LoadModelTypes(::g_pVAOMeshManager, program);
	LoadModelsIntoScene(::g_vec_pObjectsToDraw);

	std::vector<cMeshObject*> vec_pTransparentObject;
	std::vector<cMeshObject*> vec_pSolidObject;

	for (cMeshObject* m : g_vec_pObjectsToDraw)
	{
		if (m->materialDiffuse.a < 1.0f)
		{
			vec_pTransparentObject.push_back(m);
		}
		else
		{
			vec_pSolidObject.push_back(m);
		}
	}

	BubbleSort(vec_pTransparentObject);

	CreateLights(program);

	cLightHelper* pLightHelper = new cLightHelper();

	LoadTerrainAABB();

	// Get the current time to start with
	double lastTime = glfwGetTime();

	// Draw the "scene" (run the program)
	while (!glfwWindowShouldClose(window))
	{
		// Switch to the shader we want
		::pShaderManager->useShaderProgram("BasicUberShader");

		float ratio;
		int width, height;

		glm::mat4x4 matProjection = glm::mat4(1.0f);
		glm::mat4x4	matView = glm::mat4(1.0f);

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glEnable(GL_DEPTH);			// Enables the KEEPING of the depth information
		glEnable(GL_DEPTH_TEST);	// When drawing, checked the existing depth
		glEnable(GL_CULL_FACE);		// Discared "back facing" triangles

		// Colour and depth buffers are TWO DIFF THINGS.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// FOV, Aspect ratio, Near clipping plane, Far clipping plane
		matProjection = glm::perspective(0.6f, ratio, 0.1f, 10000.0f);

		matView = glm::lookAt(::g_pCamera->eye, ::g_pCamera->getAtInWorldSpace(), ::g_pCamera->getUpVector());

		glUniform3f(eyeLocation_location, ::g_pCamera->eye.x, ::g_pCamera->eye.y, ::g_pCamera->eye.z);

		glUniformMatrix4fv(matView_location, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(matProj_location, 1, GL_FALSE, glm::value_ptr(matProjection));

		for (sLight* light : g_Lights)
		{
			glUniform4f(light->position_UniLoc, light->position.x,
				light->position.y, light->position.z, 1.0f);

			glUniform4f(light->diffuse_UniLoc, light->diffuse.x,
				light->diffuse.y, light->diffuse.z, 1.0f);

			glUniform4f(light->param2_UniLoc, 1.0f, 0.0f, 0.0f, 0.0f);

			glUniform4f(light->atten_UniLoc, light->atten.x,
				light->atten.y, light->atten.z, light->atten.w);

			cMeshObject* pDebugSphere = findObjectByFriendlyName("DebugSphere");
			pDebugSphere->bIsVisible = true;
			pDebugSphere->bDontLight = true;

			glm::vec4 oldDiffuse = pDebugSphere->materialDiffuse;
			glm::vec3 oldScale = pDebugSphere->nonUniformScale;

			pDebugSphere->setDiffuseColour(glm::vec3(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f));
			pDebugSphere->bUseVertexColour = false;
			pDebugSphere->position = glm::vec3(light->position);
			glm::mat4 matBall(1.0f);

			pDebugSphere->materialDiffuse = oldDiffuse;
			pDebugSphere->setUniformScale(0.5f);
			DrawObject(pDebugSphere, matBall, program);

			if (::g_bDrawDebugLightSpheres)
			{
				DrawDebugLightSpheres(pLightHelper, light, pDebugSphere, matBall, program, oldDiffuse, oldScale);
			}
		}//for ( sLight* light : g_Lights

		{
			 // Draw the skybox first 
			cMeshObject* pSkyBox = findObjectByFriendlyName("SkyBoxObject");
			pSkyBox->position = g_pCamera->eye;
			pSkyBox->bIsVisible = true;
			pSkyBox->bIsWireFrame = false;

			// Bind the cube map texture to the cube map in the shader
			GLuint cityTextureUNIT_ID = 30;			// Texture unit go from 0 to 79
			glActiveTexture(cityTextureUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

			int cubeMapTextureID = ::g_pTextureManager->getTextureIDFromName("CityCubeMap");

			// Cube map is now bound to texture unit 30
			//glBindTexture( GL_TEXTURE_2D, cubeMapTextureID );
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

			//uniform samplerCube textureSkyBox;
			GLint skyBoxCubeMap_UniLoc = glGetUniformLocation(program, "textureSkyBox");
			glUniform1i(skyBoxCubeMap_UniLoc, cityTextureUNIT_ID);

			//uniform bool useSkyBoxTexture;
			GLint useSkyBoxTexture_UniLoc = glGetUniformLocation(program, "useSkyBoxTexture");
			glUniform1f(useSkyBoxTexture_UniLoc, (float)GL_TRUE);

			glm::mat4 matIdentity = glm::mat4(1.0f);
			DrawObject(pSkyBox, matIdentity, program);

			pSkyBox->bIsVisible = false;
			glUniform1f(useSkyBoxTexture_UniLoc, (float)GL_FALSE);
		}

		// Draw all the solid objects in the "scene"
		for (unsigned int objIndex = 0; objIndex != (unsigned int)vec_pSolidObject.size(); objIndex++)
		{
			cMeshObject* pCurrentMesh = vec_pSolidObject[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);

			DrawObject(pCurrentMesh, matModel, program);
		}

		// Draw all the transparent objects in the "scene"
		for (unsigned int objIndex = 0; objIndex != (unsigned int)vec_pTransparentObject.size(); objIndex++)
		{
			cMeshObject* pCurrentMesh = vec_pTransparentObject[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);

			DrawObject(pCurrentMesh, matModel, program);
		}

		// High res timer (likely in ms or ns)
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		// **********************************************
		{// START OF: AABB debug stuff
			//HACK: Draw Debug AABBs...

			// Get that from FindObjectByID()
			cMeshObject* pStartBallT = findObjectByFriendlyName("StartT");
			cMeshObject* pStartBallA = findObjectByFriendlyName("StartA");
			// Highlight the AABB that the rabbit is in (Or the CENTRE of the rabbit, anyway)

			float sideLength = 20.0f;
			unsigned long long AABBIdStart = cAABB::generateID(pStartBallT->position, sideLength);

			// Is there a box here? 
			std::map< unsigned long long /*ID of the AABB*/, cAABB* >::iterator itAABB_Start
				= ::g_pTerrain->m_mapAABBs.find(AABBIdStart);

			// Is there an AABB there? 
			if (itAABB_Start != ::g_pTerrain->m_mapAABBs.end())
			{
				// Yes, then get the triangles and do narrow phase collision
				CheckCollision(pStartBallT, pStartBallA, itAABB_Start->second->vecTriangles);
			}

			cMeshObject* pEndBallT = findObjectByFriendlyName("EndT");
			cMeshObject* pEndBallA = findObjectByFriendlyName("EndA");
			unsigned long long AABBIdEnd = cAABB::generateID(pEndBallT->position, sideLength);

			std::map< unsigned long long /*ID of the AABB*/, cAABB* >::iterator itAABB_End = ::g_pTerrain->m_mapAABBs.find(AABBIdEnd);

			// Is there an AABB there? 
			if (itAABB_End != ::g_pTerrain->m_mapAABBs.end())
			{
				// Yes, then get the triangles and do narrow phase collision
				CheckCollision(pEndBallT, pEndBallA, itAABB_End->second->vecTriangles);
			}

			std::map< unsigned long long /*ID of the AABB*/, cAABB* >::iterator itAABB
				= ::g_pTerrain->m_mapAABBs.begin();
			for (; itAABB != ::g_pTerrain->m_mapAABBs.end(); itAABB++)
			{
				cAABB* pCurrentAABB = itAABB->second;

				glm::vec3 cubeCorners[6];

				cubeCorners[0] = pCurrentAABB->getMinXYZ();
				cubeCorners[1] = pCurrentAABB->getMinXYZ();
				cubeCorners[2] = pCurrentAABB->getMinXYZ();
				cubeCorners[3] = pCurrentAABB->getMinXYZ();
				cubeCorners[4] = pCurrentAABB->getMinXYZ();
				cubeCorners[5] = pCurrentAABB->getMinXYZ();

				// Max XYZ
				cubeCorners[1].x += pCurrentAABB->getSideLength();
				cubeCorners[1].y += pCurrentAABB->getSideLength();
				cubeCorners[1].z += pCurrentAABB->getSideLength();

				cubeCorners[2].x += pCurrentAABB->getSideLength();

				cubeCorners[3].y += pCurrentAABB->getSideLength();

				cubeCorners[4].z += pCurrentAABB->getSideLength();

				// TODO: And the other corners... 
				cubeCorners[5].x += pCurrentAABB->getSideLength();
				cubeCorners[5].y += pCurrentAABB->getSideLength();

				// Draw line from minXYZ to maxXYZ
				::g_pDebugRenderer->addLine(cubeCorners[0], cubeCorners[1],
					glm::vec3(1, 1, 1), 0.0f);
			}
		}// END OF: Scope for aabb debug stuff
		// ********************************************************************************

		//::g_pDebugRendererACTUAL->RenderDebugObjects(matView, matProjection, deltaTime);

		UpdateWindowTitle(window);

		glfwSwapBuffers(window);		// Shows what we drew

		glfwPollEvents();

		ProcessAsyncKeys(window);

		ProcessAsyncMouse(window);

	}//while (!glfwWindowShouldClose(window))

	// Delete stuff
	delete pShaderManager;
	delete ::g_pVAOMeshManager;
	delete ::g_pTextureManager;

	delete ::g_pDebugRenderer;
	delete ::g_pCamera;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void UpdateWindowTitle(GLFWwindow* window)
{
	// #include sstream 
	//std::stringstream ssTitle;

	//ssTitle << "Light[" << g_LightIndex << "] Position: " << g_Lights[g_LightIndex]->position.x
	//	<< ", " << g_Lights[g_LightIndex]->position.y << ", " << g_Lights[g_LightIndex]->position.z
	//	<< " Attenuation: " << g_Lights[g_LightIndex]->atten.x << ", " << g_Lights[g_LightIndex]->atten.y
	//	<< ", " << g_Lights[g_LightIndex]->atten.z;

	//	glfwSetWindowTitle(window, ssTitle.str().c_str());

	return;
}

cMeshObject* findObjectByFriendlyName(std::string theNameToFind)
{
	for (unsigned int index = 0; index != g_vec_pObjectsToDraw.size(); index++)
	{
		if (g_vec_pObjectsToDraw[index]->friendlyName == theNameToFind)
		{
			return g_vec_pObjectsToDraw[index];
		}
	}

	return NULL;
}


cMeshObject* findObjectByUniqueID(unsigned int ID_to_find)
{
	for (unsigned int index = 0; index != g_vec_pObjectsToDraw.size(); index++)
	{
		if (g_vec_pObjectsToDraw[index]->getUniqueID() == ID_to_find)
		{
			return g_vec_pObjectsToDraw[index];
		}
	}

	return NULL;
}


void DrawDebugLightSpheres(cLightHelper* pLightHelper, sLight* light, cMeshObject* pDebugSphere, glm::mat4 matBall, GLuint program, glm::vec4 oldDiffuse, glm::vec3 oldScale)
{
	const float ACCURACY_OF_DISTANCE = 0.0001f;
	const float INFINITE_DISTANCE = 10000.0f;

	float distance90Percent =
		pLightHelper->calcApproxDistFromAtten(0.90f, ACCURACY_OF_DISTANCE,
			INFINITE_DISTANCE,
			light->atten.x,
			light->atten.y,
			light->atten.z);

	pDebugSphere->setUniformScale(distance90Percent);			// 90% brightness
	pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 1.0f, 0.0f));
	DrawObject(pDebugSphere, matBall, program);

	float distance50Percent =
		pLightHelper->calcApproxDistFromAtten(0.50f, ACCURACY_OF_DISTANCE,
			INFINITE_DISTANCE,
			light->atten.x,
			light->atten.y,
			light->atten.z);
	pDebugSphere->setUniformScale(distance50Percent);	// 50% brightness
	pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
	DrawObject(pDebugSphere, matBall, program);

	float distance25Percent =
		pLightHelper->calcApproxDistFromAtten(0.25f, ACCURACY_OF_DISTANCE,
			INFINITE_DISTANCE,
			light->atten.x,
			light->atten.y,
			light->atten.z);
	pDebugSphere->setUniformScale(distance25Percent);	// 25% brightness
	pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
	DrawObject(pDebugSphere, matBall, program);

	float distance1Percent =
		pLightHelper->calcApproxDistFromAtten(0.01f, ACCURACY_OF_DISTANCE,
			INFINITE_DISTANCE,
			light->atten.x,
			light->atten.y,
			light->atten.z);
	pDebugSphere->setUniformScale(distance1Percent);	// 1% brightness
	pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 0.0f, 1.0f));
	DrawObject(pDebugSphere, matBall, program);

	pDebugSphere->materialDiffuse = oldDiffuse;
	pDebugSphere->nonUniformScale = oldScale;
	pDebugSphere->bIsVisible = false;
}

// An optimized version of Bubble Sort 
void BubbleSort(std::vector<cMeshObject*>& vec)
{
	int i, j;
	bool swapped;
	for (i = 0; i < vec.size(); i++)
	{
		swapped = false;
		for (j = 0; j < vec.size() - 1; j++)
		{
			if (glm::distance(g_pCamera->eye, vec[j]->position) < glm::distance(g_pCamera->eye, vec[j + 1]->position))
			{
				cMeshObject* temp = vec[j];
				vec[j] = vec[j+1];
				vec[j+1] = temp;

				swapped = true;
			}
		}

		// IF no two elements were swapped by inner loop, then break 
		if (swapped == false)
			break;
	}
}

void LoadTerrainAABB(void)
{
	// *******
	// This REALLY should be inside the cAABBHierarchy, likely... 
	// *******


	// Read the graphics mesh object, and load the triangle info
	//	into the AABB thing.
	// Where is the mesh (do the triangles need to be transformed)??

	cMeshObject* pTerrain = findObjectByFriendlyName("Terrain");

	sModelDrawInfo terrainMeshInfo;
	terrainMeshInfo.meshFileName = pTerrain->meshName;

	::g_pVAOMeshManager->FindDrawInfoByModelName(terrainMeshInfo);


	// How big is our AABBs? Side length?
	float sideLength = 20.0f;		// Play with this lenght
									// Smaller --> more AABBs, fewer triangles per AABB
									// Larger --> More triangles per AABB

	for (unsigned int triIndex = 0; triIndex != terrainMeshInfo.numberOfTriangles; triIndex++)
	{
		// for each triangle, for each vertex, determine which AABB the triangle should be in
		// (if your mesh has been transformed, then you need to transform the tirangles 
		//  BEFORE you do this... or just keep the terrain UNTRANSFORMED)

		sPlyTriangle currentTri = terrainMeshInfo.pTriangles[triIndex];

		sPlyVertex currentVerts[3];
		currentVerts[0] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_1];
		currentVerts[1] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_2];
		currentVerts[2] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_3];

		// This is the structure we are eventually going to store in the AABB map...
		cAABB::sAABB_Triangle curAABBTri;
		curAABBTri.verts[0].x = currentVerts[0].x;
		curAABBTri.verts[0].y = currentVerts[0].y;
		curAABBTri.verts[0].z = currentVerts[0].z;
		curAABBTri.verts[1].x = currentVerts[1].x;
		curAABBTri.verts[1].y = currentVerts[1].y;
		curAABBTri.verts[1].z = currentVerts[1].z;
		curAABBTri.verts[2].x = currentVerts[2].x;
		curAABBTri.verts[2].y = currentVerts[2].y;
		curAABBTri.verts[2].z = currentVerts[2].z;

		// Is the triangle "too big", and if so, split it (take centre and make 3 more)
		// (Pro Tip: "too big" is the SMALLEST side is greater than HALF the AABB length)
		// Use THOSE triangles as the test (and recursively do this if needed),
		// +++BUT+++ store the ORIGINAL triangle info NOT the subdivided one
		// 
		// For the student to complete... 
		// 


		for (unsigned int vertIndex = 0; vertIndex != 3; vertIndex++)
		{
			// What AABB is "this" vertex in? 
			unsigned long long AABB_ID =
				cAABB::generateID(curAABBTri.verts[0],
					sideLength);

			// Do we have this AABB alredy? 
			std::map< unsigned long long/*ID AABB*/, cAABB* >::iterator itAABB
				= ::g_pTerrain->m_mapAABBs.find(AABB_ID);

			if (itAABB == ::g_pTerrain->m_mapAABBs.end())
			{
				// We DON'T have an AABB, yet
				cAABB* pAABB = new cAABB();
				// Determine the AABB location for this point
				// (like the generateID() method...)
				glm::vec3 minXYZ = curAABBTri.verts[0];
				minXYZ.x = (floor(minXYZ.x / sideLength)) * sideLength;
				minXYZ.y = (floor(minXYZ.y / sideLength)) * sideLength;
				minXYZ.z = (floor(minXYZ.z / sideLength)) * sideLength;

				pAABB->setMinXYZ(minXYZ);
				pAABB->setSideLegth(sideLength);
				// Note: this is the SAME as the AABB_ID...
				unsigned long long the_AABB_ID = pAABB->getID();

				::g_pTerrain->m_mapAABBs[the_AABB_ID] = pAABB;

				// Then set the iterator to the AABB, by running find again
				itAABB = ::g_pTerrain->m_mapAABBs.find(the_AABB_ID);
			}//if( itAABB == ::g_pTheTerrain->m_mapAABBs.end() )

			// At this point, the itAABB ++IS++ pointing to an AABB
			// (either there WAS one already, or I just created on)

			itAABB->second->vecTriangles.push_back(curAABBTri);

		}//for ( unsigned int vertIndex = 0;

	}//for ( unsigned int triIndex



	// At runtime, need a "get the triangles" method...

	return;
}

