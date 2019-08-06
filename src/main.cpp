#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>


# define M_PI           3.14159265358979323846

int PERSPECTIVE = 800 * 0.8;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 1.f;
const float ZOOM = 90.0f;


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
		glm::vec3 front;
		front.x = 1* cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = 1*sin(glm::radians(Pitch)) ;
		front.z = 1*cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));;
		
		Position += front;
		// Also re-calculate the Right and Up vector
		//Right = glm::normalize(glm::cross(Position, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		//Up = glm::normalize(glm::cross(Right, Position));
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles

	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
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
							, ((x>3 && map[x + y * 18] == '+') ? x-3 : 0)
							, 0
							, 1
							,sf::Sprite(texture, font.getGlyph(map[x + y * 18], 24, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250, 205, 195));
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , float z, Camera cam , glm::vec3 target)
{
	// x = x * 24  - cam.x ;
	// y = y * 24  - cam.y ;

		/*glm::mat4 mPerspective =
	{
		 S , 0 , 0 , 0,
		 0 , S , 0 , 0,
		 0 , 0 , -(far /(far - near)) ,-1 ,
		 0 , 0 , -((far * near) / (far - near)) , 0
	};
	*/

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


	 */

	

	glm::vec4 r2orig = {x+24, y+24, z, 1};
	float fov = 100.0f;
	double S = 1 / (glm::tan((fov / 2) * (M_PI / 180)));
	float near =  0.1;
	float far = 100;

	glm::mat4 mScale =
	{
		 24, 0  , 0,  0,
		 0 , 24, 0,  0,
		 0 , 0 ,  24  , 0,
		 0 , 0,   0,  1
	};

	
	glm::vec4 orig = { x, y, z ,1};
	// calculate the model matrix for each object and pass  before drawing
	 //model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first;
	 glm::mat4 model = mScale;

	//auto camPos = glm::vec3(cam.x, cam.z, cam.z) * 23;
	glm::mat4 mView = glm::lookAt(
		cam.Position, // Camera in World Space
		{target.x, target.y, target.z}, // and looks at the target
		{0,1,0}//cam.Up // Head is up (set to 0,-1,0 to look upside-down)
	);

	auto mProjection = glm::perspective( glm::radians(fov), 800.0f / 600.0f, near,far) ;


	auto MVPmatric = mProjection * mView * model;

	auto mFinal = MVPmatric * orig;

	//float xp = view.x * cosf(cam.rotZ) - (view.y ) * sinf(cam.rotZ);
	//float yp = (view.x ) * sinf(cam.rotZ) + (view.y ) * cosf(cam.rotZ);
	//scale = { mFinal2.x - mFinal.x,mFinal2.y - mFinal.y };
	sf::Vector2f vec(mFinal.x  +800/2, mFinal.y + 600 / 2);
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
	Camera camera = { };
	camera.WorldUp = {0,1,0};
	camera.Front = { 0,0,0 };
	camera.MovementSpeed = SPEED;
	camera.MouseSensitivity = SENSITIVITY;
	camera.Zoom = ZOOM;
	camera.updateCameraVectors();
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
				//std::cout << "wheel movement: " <<  << std::endl
			}
		}
		player.z = list.at(player.x + player.y * 18).z;
		camera.Position = { player.x * 24  , player.y * 24, player.z * 24  };
		camera.updateCameraVectors();
		
		glm::vec3 target = { player.x * 24, player.y * 24, player.z * 24 };
		window.clear(sf::Color::Black);
		//std::cout << camera.rotZ << std::endl;
		
		for (Tile ent : list)
		{
			ent.sprite.setPosition(to_global(ent.x,ent.y, ent.z,camera, target));
			//ent.sprite.setRotation(glm::radians(camera.Pitch));
			ent.sprite.scale(1,1);
			window.draw(ent.sprite);
		}
		//std::cout << camera.x << ":"  << camera.y<< std::endl;
		player.sprite.setPosition(to_global(player.x, player.y,player.z, camera, target));
		//player.sprite.setRotation(camera.rotZ * (180.f / M_PI));
		player.sprite.setScale(1, 1);
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}