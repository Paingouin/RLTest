#pragma once
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "glm/ext/matrix_clip_space.hpp" // glm::perspective

struct Camera
{
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 LastPosition;

	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float fov = 70.0f;
	//double S = 1.0 / (glm::tan(fov / 2.0));
	float near = 1.f;
	float far = 100.f;

	glm::mat4 model;
	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::vec4 viewport = { 0.0f , 0.0f, 800.0f,600.0f };

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors(glm::vec3 target)
	{
		// Calculate the new Front vector
		glm::vec3 posOffset;
		posOffset.x = Zoom * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		posOffset.y = Zoom * sin(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		posOffset.z = Zoom * cos(glm::radians(Yaw));

		Position += posOffset;

		glm::mat4 mScale =
		{
			 1.f , 0.f, 0.f, 0.f,
			 0.f , 1.f, 0.f, 0.f,
			 0.f , 0.f, 1.f, 0.f,
			 0.f , 0.f, 0.f, 1.f
		};

		// calculate the model matrix for each object and pass  before drawing
		 //model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first;
		model = mScale;
		glm::vec4 camPos = { Position.x , Position.y , Position.z  , 1.f };
		glm::vec4 tarPos = { target.x , target.y , target.z  , 1.f };


		glm::vec3 camPos3 = model * camPos;
		glm::vec3 tarPos3 = model * tarPos;
		//std::cout << "before cam:" << camPos3.x << " : " << camPos3.y << " : " << camPos3.z << std::endl;
		mView = glm::lookAtRH(
			camPos3, // Camera in World Space  
			tarPos3, // and looks at the target
			{ 0.f, 0.f, 1.f }// Head is up (set to 0,-1,0 to look upside-down)
		);

		//NO : z is Normalized  [-1 +1]
		//ZO : z is normalied [0 +1]
		mProjection = glm::perspectiveRH_NO(glm::radians(fov), 800.f / 600.f, near, far);
		mProjection = mProjection * mView * model;
		
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//std::cout << xoffset << std::endl;
		Yaw += yoffset;
		Pitch -= xoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Yaw > 90.0f)
				Yaw = 90.0f;
			if (Yaw < 0.1f)
				Yaw = 0.1f;
		}
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= yoffset;


		if (Zoom >= 1.0f && Zoom <= 90.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 90.0f)
			Zoom = 90.0f;
	}


	//Calculate Global position on screen based of camera
	const sf::Vector2f to_global( float x, float y, float z, float& winX, float& winY, float& scale, bool& render)
	{

		glm::vec4 orig = { x, y, z , 1.f };
		glm::vec4 mFinal =mProjection  * orig;
		float distance = mFinal.w;
		mFinal.x /= mFinal.w;
		mFinal.y /= mFinal.w;
		mFinal.z = glm::abs(mFinal.z) / mFinal.w;
		scale = (((6.f * 0.8f * (800.f / 600.f)) / distance) * (800.f / 600.f));
		if (mFinal.z < -1.f || mFinal.z > 1.f) //culling
		{
			render = false;
			return sf::Vector2f();
		}
		else
		{
			render = true;
		}

		mFinal = mFinal * 0.5f + 0.5f;
		//To do Z0(0,1) (NO par default) :  comment above and uncomment below
		//mFinal.x = mFinal.x * 0.5 + 0.5;
		//mFinal.y = mFinal.y * 0.5 + 0.5;

		winX = (mFinal.x * viewport[2]) + viewport[0];
		winY = (viewport[3] - (mFinal.y * viewport[3])) + viewport[1];

		const sf::Vector2f vec(mFinal.x, mFinal.y);//return final position
		return vec;
	}

};