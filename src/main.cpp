#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>

#define  GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include  "glm/ext/matrix_projection.hpp"


#include <iostream>



# define M_PI           3.14159265358979323846

int PERSPECTIVE = 800 * 0.8;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.5f;
const float ZOOM = 30.0f;


//NOTE : do depth testung with scaling, and a vector sort by overloading <

//"angle" of the keypress, subtract the camera Z rotation, find the nearest matching direction

typedef unsigned char asciiChar;

typedef uint8_t		uint8;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef int32_t		int32;
typedef int64_t		int64;

#define internal static
#define local_persist static
#define global_variable static

global_variable float scale = 1;

struct Tile
{
	char glyph;

	int32 x, y, z;
	int32 color;
	float scaleFactor;
	sf::Sprite sprite;
	//add scale factor for depth testing 
};


struct Player
{
	char glyph;

	int32 x, y, z;
	int32 color;
	sf::Sprite sprite;
};

struct Camera
{
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 posOffset;
		posOffset.x = Zoom*sin(glm::radians(Yaw)) * sin(glm::radians(Pitch));
		posOffset.y = Zoom*cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		posOffset.z = Zoom*cos(glm::radians(Yaw))  ;
		
		glm::vec3 front = glm::normalize(Position + posOffset);

		Position += posOffset;
		//std::cout << Position.x << " : " << Position.y  << " : "<< Position.z <<std::endl;

		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, front));
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = false)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += yoffset;
		Pitch += xoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Yaw > 89.0f)
				Yaw = 89.0f;
			if (Yaw < 0)
				Yaw = 0;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles

	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= yoffset;


	/*	if (Zoom >= 1.0f && Zoom <= 90.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 90.0f)
			Zoom = 90.0f;*/
	}
};


struct Controller
{
	int32 lastPosMouseX, lastPosMouseZ;
};

char map[18 * 18] = {
	'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
};



std::vector<Tile> gensprite_map( sf::Font& font,const sf::Texture& texture ,char* map)
{
	std::vector<Tile> list_entities;
	for (int y = 0; y < 18; ++y)
	{
		for (int x = 0; x < 18; ++x)
		{
			Tile ent = {
							map[x + y * 18]
							, x 
							, y 
							, ((x>3 && map[x + y * 18] == '+') ? ((x-3)) : 0)
							, 0
							, 1
							,sf::Sprite(texture, font.getGlyph(map[x + y * 18], 24, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250, 205, 195));
			ent.sprite.setOrigin(11, 11);
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , float z, Camera cam , glm::vec3 target, float& scale, bool& render)
{
	// x = x * 24  - cam.x ;
	// y = y * 24  - cam.y ;

	/*
	 glm::mat4 mRotationX =
	{
		1 , 0 ,  0, 0,
	//	0,   cosf(cam.rotX),-sinf(cam.rotX), 0,
	//	 0,  sinf(cam.rotX), cosf(cam.rotZ),0,
		 0 , 0,  0, 1
	};

	 glm::mat4 mRotationZ =
	 {
	//	 cosf(cam.rotZ) , sinf(cam.rotZ) ,  0, 0,
	//	 -sinf(cam.rotZ) ,  cosf(cam.rotZ),  0 , 0,
		  0, 0, 1,0,
		  0 , 0,   0, 1
	 };

		glm::mat4 mTranslate =
	{
		 1 , 0 , 0 , 0,
		 0 , 1 , 0 , 0,
		 0 , 0 , 1 , 0,
		 -cam.Position.x , -cam.Position.y , 0 , 1
	 };
	 glm::mat4 mPerspective =
	{
		 S*(800.0/600.0) , 0 , 0 , 0,
		 0 , S , 0 , 0,
		 0 , 0 , ((far +near) / (far - near)) ,1 ,
		 0 , 0 ,( (2* far * near) / (near - far)) , 0
	};

	 */

	
	float fov =75.0f;
	double S = 1.0 / (glm::tan(fov / 2.0));
	float near =  0.1f;
	float far = 100.f;
	
	glm::mat4 mScale =
	{
		 24 , 0, 0, 0,
		 0 , 24, 0, 0,
		 0 , 0, 24, 0,
		 0 , 0, 0, 1
	};
	
	glm::vec4 orig = { x, y, z , 0};

	// calculate the model matrix for each object and pass  before drawing
	 //model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first;
	glm::mat4 model = mScale;
	glm::vec4 camPos = { cam.Position.x , cam.Position.y , cam.Position.z  , 0 };
	glm::vec4 tarPos = { target.x , target.y , target.z  , 0};

	
	glm::vec3 camPos3 = model * camPos;
	glm::vec3 tarPos3 = model * tarPos;
	//std::cout << "before cam:" << camPos3.x << " : " << camPos3.y << " : " << camPos3.z << std::endl;
	glm::mat4 mView = glm::lookAt(
		camPos3, // Camera in World Space  
		tarPos3, // and looks at the target
		{0,1,0} // Head is up (set to 0,-1,0 to look upside-down)
	);

	//NO : z is Normalized  [-1 +1]
	//ZO : z is normalied [0 +1]
	auto mProjection = glm::perspectiveNO( glm::radians(fov), 800.f/600.f, near,far) ;
	//auto mMV = mView * model;
	//auto MVPmatric = mProjection * ;
	//auto mFinal = MVPmatric * orig;
	//mFinal /= mFinal.w;//perspective divide
	glm::vec4 viewport = { 0.0 , 0.0, 800.0,600.0 };
	auto mFinal = mProjection*  mView * model * orig;

	mFinal /= mFinal.w;

	//std::cout << mFinal.z << std::endl;
	//To do Z0 (NO par default) :  comment above and uncomment below
	//mFinal.x = mFinal.x * 0.5 + 0.5;
	//mFinal.y = mFinal.y * 0.5 + 0.5;
	mFinal = mFinal * 0.5f + 0.5f;
	std::cout << mFinal.z << std::endl;

	mFinal[0] = mFinal[0] * viewport[2] + viewport[0];
	mFinal[1] = mFinal[1] * viewport[3] + viewport[1];
	if (mFinal.z > 1 || z < -1)
	{
		render = false;
		return {};
	}

	//auto mFinal = glm::project(orig, mMV, mProjection, viewport);
	const float epsi = 0.00001f;

	/*if (mFinal.z - (1.0f - epsi) < 0|| mFinal.z -(1.0f - epsi) > 1)
	{
		render = false;
	}*/
	sf::Vector2f vec(mFinal.x , mFinal.y);
	return vec;
}

int main()
{
	sf::RenderWindow  window(sf::VideoMode(800, 600), "RL test");
	float lastX = 400, lastY = 300;
	bool firstMouse = true;

	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);


	sf::Font font;
	if (!font.loadFromFile("nasalization-rg.ttf"))
	{
		// error...
	}

	const sf::Texture& texture = font.getTexture(24 );
	std::vector<Tile> list = gensprite_map(font, texture, map);
	Controller control = {};
	
	Player player =
	{
		'@'
		, 2 
		, 6 
		, 0
		, 0
		,sf::Sprite(texture, font.getGlyph('@', 24 , false).textureRect)
	};
	player.sprite.setOrigin(11, 11);
	Camera camera = { };
	camera.WorldUp = {0,1,0};
	camera.Pitch = 0;
	camera.Yaw = 0.f;
	camera.Front = { 0,0,0 };
	camera.MovementSpeed = SPEED;
	camera.MouseSensitivity = SENSITIVITY;
	camera.Zoom = ZOOM;
	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				player.x -= 1;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				player.x += 1;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				player.y -= 1;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				player.y += 1;
			}

			if (event.type == sf::Event::MouseMoved && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
			{

				if (firstMouse)
				{
					lastX = event.mouseMove.x;
					lastY = event.mouseMove.y;
					firstMouse = false;
				}

				float xoffset = event.mouseMove.x - lastX;
				float yoffset = lastY - event.mouseMove.y; // reversed since y-coordinates go from bottom to top

				lastX = event.mouseMove.x;
				lastY = event.mouseMove.y;

				camera.ProcessMouseMovement(xoffset, yoffset);
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				camera.ProcessMouseScroll(event.mouseWheelScroll.delta);
			}
		}
		player.z = list.at(player.x + player.y * 18.0).z;
		camera.Position = { player.x , player.y , player.z  };
		camera.updateCameraVectors();
		glm::vec3 target = { player.x , player.y , player.z };
		window.clear(sf::Color::Black);

		//std::cout << camera.rotZ << std::endl;	
		bool render = true;
		for (Tile ent : list)
		{
			render = true;
			scale = 1;
			ent.sprite.setPosition(to_global(ent.x,ent.y, ent.z,camera, target ,scale, render));
			if (!render) continue;
			//ent.sprite.setRotation();
			ent.sprite.setScale(scale,scale);
			window.draw(ent.sprite);
		}
		//std::cout << camera.x << ":"  << camera.y<< std::endl;
		player.sprite.setPosition(to_global(player.x, player.y,player.z, camera, target, scale,render));
		//player.sprite.setRotation(camera.rotZ * (180.f / M_PI));
		player.sprite.setScale(1, 1);
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}