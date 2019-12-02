#include "globalOpenGLStuff.h"
#include "globalStuff.h"

// This has all the keyboard, mouse, and controller stuff
// No modifiers		Camera controls
// Shift			Camera rotations
// Ctrl				Model controls
// Alt				Light controls

bool IsShiftDown(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) { return true; }
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) { return true; }
	// both are up
	return false;
}

bool IsCtrlDown(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) { return true; }
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) { return true; }
	// both are up
	return false;
}

bool IsAltDown(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT)) { return true; }
	if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT)) { return true; }
	// both are up
	return false;
}

bool AreAllModifiersUp(GLFWwindow* window)
{
	if (IsShiftDown(window)) { return false; }
	if (IsCtrlDown(window)) { return false; }
	if (IsAltDown(window)) { return false; }
	// Yup, they are all UP
	return true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// Model controls
	if (mods == GLFW_MOD_CONTROL)
	{

		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		{
			if (::g_ModelIndex + 1 < ::g_vec_pObjectsToDraw.size())
			{
				::g_ModelIndex++;
			}
			else
			{
				::g_ModelIndex = 0;
			}

		}

		if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
		{
			::g_vec_pObjectsToDraw[g_ModelIndex]->bIsWireFrame = !g_vec_pObjectsToDraw[g_ModelIndex]->bIsWireFrame;
		}

		if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		{
			::g_vec_pObjectsToDraw[g_ModelIndex]->vecTextures[0].strength += 0.5f;
			::g_vec_pObjectsToDraw[g_ModelIndex]->vecTextures[1].strength -= 0.5f;
		}
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			::g_vec_pObjectsToDraw[g_ModelIndex]->vecTextures[0].strength -= 0.5f;
			::g_vec_pObjectsToDraw[g_ModelIndex]->vecTextures[1].strength += 0.5f;
		}


	}//if ( isShiftDownAlone(mods) )

	// Light controls
	if (mods == GLFW_MOD_ALT)
	{

		if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
		{
			// Toggle the spheres on and off
			//::g_bDrawDebugLightSpheres = !::g_bDrawDebugLightSpheres;
			if (::g_LightIndex + 1 == ::g_Lights.size())
			{
				::g_LightIndex = 0;
			}
			else
			{
				::g_LightIndex++;
			}
		}
	}

	return;
}

void ProcessAsyncKeys(GLFWwindow* window)
{
	const float MOVE_SPEED_SLOW = 0.1f;
	const float MOVE_SPEED_FAST = 1.0f;
	const float CAMERA_TURN_SPEED = 0.1f;

	float cameraMoveSpeed = ::g_pCamera->movementSpeed;
	float moveSpeed = MOVE_SPEED_SLOW;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		moveSpeed = MOVE_SPEED_FAST;
	}

	// If no keys are down, move the camera
	if (AreAllModifiersUp(window))
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			::g_vec_pObjectsToDraw[2]->position.z += moveSpeed;
			::g_vec_pObjectsToDraw[3]->position.z += moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	// "backwards"
		{
			::g_vec_pObjectsToDraw[2]->position.z -= moveSpeed;
			::g_vec_pObjectsToDraw[3]->position.z -= moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	// "left"
		{
			::g_vec_pObjectsToDraw[2]->position.x += moveSpeed;
			::g_vec_pObjectsToDraw[3]->position.x += moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	// "right"
		{
			::g_vec_pObjectsToDraw[2]->position.x -= moveSpeed;
			::g_vec_pObjectsToDraw[3]->position.x -= moveSpeed;
		}
	}//if(AreAllModifiersUp(window)

	if (IsShiftDown(window))
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			::g_vec_pObjectsToDraw[4]->position.z += moveSpeed;
			::g_vec_pObjectsToDraw[5]->position.z += moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	// "backwards"
		{
			::g_vec_pObjectsToDraw[4]->position.z -= moveSpeed;
			::g_vec_pObjectsToDraw[5]->position.z -= moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	// "left"
		{
			::g_vec_pObjectsToDraw[4]->position.x += moveSpeed;
			::g_vec_pObjectsToDraw[5]->position.x += moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	// "right"
		{
			::g_vec_pObjectsToDraw[4]->position.x -= moveSpeed;
			::g_vec_pObjectsToDraw[5]->position.x -= moveSpeed;
		}
	}//IsShiftDown(window)


		// Control (ctrl) key down? Move light
	if (IsCtrlDown(window))
	{
				// Note: The "== GLFW_PRESS" isn't really needed as it's actually "1" 
		// (so the if() treats the "1" as true...)


		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			//			g_CameraEye.z += cameraSpeed;
			::g_pCamera->MoveForward_Z(+cameraMoveSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	// "backwards"
		{
			//			g_CameraEye.z -= cameraSpeed;
			::g_pCamera->MoveForward_Z(-cameraMoveSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	// "left"
		{
			//			g_CameraEye.x -= cameraSpeed;
			::g_pCamera->MoveLeftRight_X(-cameraMoveSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	// "right"
		{
			//			g_CameraEye.x += cameraSpeed;
			::g_pCamera->MoveLeftRight_X(+cameraMoveSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)	// "up"
		{
			::g_pCamera->MoveUpDown_Y(-cameraMoveSpeed);
			//			::g_pFlyCamera->Roll_CW_CCW( +cameraSpeed );
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)	// "down"
		{
			//			g_CameraEye.y -= cameraSpeed;
			::g_pCamera->MoveUpDown_Y(+cameraMoveSpeed);
			//			::g_pFlyCamera->Roll_CW_CCW( -cameraSpeed );
		}

	}//if(!IsShiftDown(window) )	


	const float MIN_LIGHT_BRIGHTNESS = 0.001f;

	// Control (alt) key down? Move light
	if (IsAltDown(window))
	{
		if (glfwGetKey(window, GLFW_KEY_W)) { g_Lights[g_LightIndex]->position.z += moveSpeed; }
		if (glfwGetKey(window, GLFW_KEY_S)) { g_Lights[g_LightIndex]->position.z -= moveSpeed; }
		if (glfwGetKey(window, GLFW_KEY_A)) { g_Lights[g_LightIndex]->position.x += moveSpeed; }
		if (glfwGetKey(window, GLFW_KEY_D)) { g_Lights[g_LightIndex]->position.x -= moveSpeed; }
		if (glfwGetKey(window, GLFW_KEY_Q)) { g_Lights[g_LightIndex]->position.y += moveSpeed; }
		if (glfwGetKey(window, GLFW_KEY_E)) { g_Lights[g_LightIndex]->position.y -= moveSpeed; }

		if (glfwGetKey(window, GLFW_KEY_1))
		{	// Const
			g_Lights[g_LightIndex]->atten.x *= 0.99f;		// Decrease by 1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_2))
		{	// Const
			g_Lights[g_LightIndex]->atten.x *= 1.01f;		// Increase by 1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_3))
		{	// Linear
			g_Lights[g_LightIndex]->atten.y *= 0.99f;		// Decrease by 1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_4))
		{	// Linear
			g_Lights[g_LightIndex]->atten.y *= 1.01f;		// Increase by 1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_5))
		{	// Quadratic
			g_Lights[g_LightIndex]->atten.z *= 0.99f;		// Decrease by 0.1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_6))
		{	// Quadratic
			g_Lights[g_LightIndex]->atten.z *= 1.01f;		// Increase by 0.1 percent
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			std::cout << "Light[" << g_LightIndex << "] Position: " << g_Lights[g_LightIndex]->position.x
				<< ", " << g_Lights[g_LightIndex]->position.y << ", " << g_Lights[g_LightIndex]->position.z
				<< " Attenuation: " << g_Lights[g_LightIndex]->atten.x << ", " << g_Lights[g_LightIndex]->atten.y
				<< ", " << g_Lights[g_LightIndex]->atten.z << std::endl;
		}

	}//if(!IsShiftDown(window) )

	return;
}

bool g_MouseIsInsideWindow = false;

void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		::g_MouseIsInsideWindow = true;
		std::cout << "Mouse moved indide window" << std::endl;
	}
	else
	{
		::g_MouseIsInsideWindow = false;
		std::cout << "Mouse moved outside window" << std::endl;
	}
	return;
}//cursor_enter_callback(...

// Mouse (cursor) callback
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

	return;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// A regular mouse wheel returns the y value
	::g_pCamera->setMouseWheelDelta(yoffset);

	//	std::cout << "Mouse wheel: " << yoffset << std::endl;

	return;
}

void ProcessAsyncMouse(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	::g_pCamera->setMouseXY(x, y);

	const float MOUSE_SENSITIVITY = 0.1f;

	//	std::cout << ::g_pFlyCamera->getMouseX() << ", " << ::g_pFlyCamera->getMouseY() << std::endl;

		// Mouse left (primary?) button pressed? 
		// AND the mouse is inside the window...
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		&& ::g_MouseIsInsideWindow)
	{
		// Mouse button is down so turn the camera
		::g_pCamera->Yaw_LeftRight(-::g_pCamera->getDeltaMouseX() * MOUSE_SENSITIVITY);

		::g_pCamera->Pitch_UpDown(::g_pCamera->getDeltaMouseY() * MOUSE_SENSITIVITY);

	}

	// Adjust the mouse speed
	if (::g_MouseIsInsideWindow)
	{
		const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

		// Adjust the movement speed based on the wheel position
		::g_pCamera->movementSpeed = ::g_pCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY;
	}

	return;
}//ProcessAsyncMouse(...
