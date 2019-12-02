#include "globalStuff.h"

int g_LightIndex = 0;

void CreateLights(GLint program)
{
	// This would happen in the "light manager", where it would 
//	do this for each light...

	{
		sLight* pTheOneLight = new sLight();

		pTheOneLight = new sLight();

		// Do this once...
		pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[0].position");
		pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[0].diffuse");
		pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[0].specular");
		pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[0].atten");
		pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[0].direction");
		pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[0].param1");
		pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[0].param2");

		pTheOneLight->position = glm::vec4(235.0f, 1110.0f, -100.0f, 1.0f);
		pTheOneLight->atten.x = 0.229731f;				// 	float constAtten = 0.0f;
		pTheOneLight->atten.y = 0.00129778f;		//	float linearAtten = 0.01f;
		pTheOneLight->atten.z = 0.00000015f;		//	float quadAtten = 0.001f;
		pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light


		pTheOneLight->SetLightType(sLight::POINT_LIGHT);
		g_Lights.push_back(pTheOneLight);
	}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	pTheOneLight = new sLight();

	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[1].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[1].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[1].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[1].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[1].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[1].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[1].param2");

	//	pTheOneLight->position = glm::vec4(-250.0f, 210.0f, 200.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.537262f;				// 	float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.00551435f;		//	float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.00000015f;		//	float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light


	//	pTheOneLight->SetLightType(sLight::POINT_LIGHT);
	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	pTheOneLight = new sLight();

	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[2].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[2].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[2].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[2].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[2].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[2].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[2].param2");

	//	pTheOneLight->position = glm::vec4(-170.0f, 140.0f, -180.0f, 1.0f);
	//	pTheOneLight->atten.x = 1.0f;				// 	float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.0172419f;		//	float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.00000015f;		//	float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light


	//	pTheOneLight->SetLightType(sLight::POINT_LIGHT);
	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	// Do this once...
	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[3].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[3].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[3].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[3].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[3].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[3].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[3].param2");

	//	pTheOneLight->position = glm::vec4(-350.0f, 190.0f, -420.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.0976686f;	// 			float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.0186726f;	//			float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.0000434692f;	//			float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	// Do this once...
	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[4].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[4].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[4].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[4].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[4].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[4].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[4].param2");

	//	pTheOneLight->position = glm::vec4(280.0f, 135.0f, 252.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.537262f;				// 	float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.00551435f;		//	float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.00000015f;		//	float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	pTheOneLight = new sLight();

	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[5].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[5].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[5].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[5].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[5].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[5].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[5].param2");

	//	pTheOneLight->position = glm::vec4(430.0f, 95.0f, -15.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.45218f;				// 	float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.0113808f;		//	float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.00000015f;		//	float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light


	//	pTheOneLight->SetLightType(sLight::POINT_LIGHT);
	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	// Do this once...
	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[6].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[6].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[6].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[6].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[6].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[6].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[6].param2");

	//	pTheOneLight->position = glm::vec4(265.0f, 150.0f, -320.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.132813f;	// 			float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.00309225f;	//			float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.0000434692f;	//			float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	//	g_Lights.push_back(pTheOneLight);
	//}

	//{
	//	sLight* pTheOneLight = new sLight();

	//	// Do this once...
	//	pTheOneLight->position_UniLoc = glGetUniformLocation(program, "theLights[7].position");
	//	pTheOneLight->diffuse_UniLoc = glGetUniformLocation(program, "theLights[7].diffuse");
	//	pTheOneLight->specular_UniLoc = glGetUniformLocation(program, "theLights[7].specular");
	//	pTheOneLight->atten_UniLoc = glGetUniformLocation(program, "theLights[7].atten");
	//	pTheOneLight->direction_UniLoc = glGetUniformLocation(program, "theLights[7].direction");
	//	pTheOneLight->param1_UniLoc = glGetUniformLocation(program, "theLights[7].param1");
	//	pTheOneLight->param2_UniLoc = glGetUniformLocation(program, "theLights[7].param2");

	//	pTheOneLight->position = glm::vec4(-56.0f, 126.0f, -370.0f, 1.0f);
	//	pTheOneLight->atten.x = 0.132813f;	// 			float constAtten = 0.0f;
	//	pTheOneLight->atten.y = 0.00309225f;	//			float linearAtten = 0.01f;
	//	pTheOneLight->atten.z = 0.0000434692f;	//			float quadAtten = 0.001f;
	//	pTheOneLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	//	g_Lights.push_back(pTheOneLight);
	//}


}
