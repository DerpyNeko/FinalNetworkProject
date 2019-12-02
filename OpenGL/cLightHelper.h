#ifndef _cLightHelper_HG_
#define _cLightHelper_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class cLightHelper
{
public:

	static const float DEFAULT_ATTEN_CONST;
	static const float DEFAULT_ATTEN_LINEAR;
	static const float DEFAULT_ATTEN_QUADRATIC;
	static const float DEFAULT_AMBIENT_TO_DIFFUSE_RATIO;

	// This is a helper function that returns a light "brightness" level
	//	based on the attenuation functions. It's closely tied to the shader
	//	so if the shader changes, you'd have to change this, too
	// The idea is that you can approximate how far an object would have 
	//	to be from the light for a given lightLevel (from 0.0 to 1.0), 
	//	within a certain accuracy. 
	// Accuracy could be something like 1% or 0.01f.
	// infiniteDistance is where there's no point in continuing the calculation. 
	//	i.e. if we still haven't hit the desired light level by infiniteDistance, it will reutrn infiniteDistance
	static const unsigned int DEFAULTMAXITERATIONS = 50;
	static const float DEFAULTINFINITEDISTANCE;
	static const float DEFAULDIFFUSEACCURACYTHRESHOLD;
	float calcApproxDistFromAtten(float targetLightLevel);
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy);
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance,
		float constAttenuation, float linearAttenuation, float quadraticAttenuation,
		unsigned int maxIterations = DEFAULTMAXITERATIONS);
	static const float DEFAULTZEROTHRESHOLD;
	// If value gets below the "zeroThreshold", will be treated as zero
	//float calcAttenFromDist(float distance, float zeroThreshold = DEFAULTZEROTHRESHOLD);
	float calcDiffuseFromAttenByDistance(float distance,
		float constAttenuation,
		float linearAttenuation,
		float quadraticAttenuation,
		float zeroThreshold = DEFAULTZEROTHRESHOLD);
};

#endif // cLightHelper
