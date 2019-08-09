#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "glm/ext/matrix_clip_space.hpp" // glm::perspective



#include <iostream>

int PERSPECTIVE = 800 * 0.8;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.5f;
const float ZOOM = 30.0f;


//NOTE : do depth testung with scaling, and a vector sort by overloading <
//Todo : vertex array for sprite bashing

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

	float x, y, z;
	int32 color;
	float scaleFactor;
	sf::Sprite sprite;
	//add scale factor for depth testing 
};


struct Player
{
	char glyph;

	float x, y, z;
	int32 color;
	sf::Sprite sprite;
};

struct Camera
{
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 LastPosition;
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

	glm::mat4 model;
	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::vec4 viewport = { 0.0f , 0.0f, 800.0f,600.0f };

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors(glm::vec3 target)
	{
		// Calculate the new Front vector
		glm::vec3 posOffset;
		posOffset.x = Zoom*sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		posOffset.y = Zoom*sin(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		posOffset.z = Zoom*cos(glm::radians(Yaw))  ;

		Position += posOffset;


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


		float fov = 75.0f;
		//double S = 1.0 / (glm::tan(fov / 2.0));
		float near = 0.1f;
		float far = 100.f;


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
			{ 0.f, 0.f, 1.f}// Head is up (set to 0,-1,0 to look upside-down)
		);

		//NO : z is Normalized  [-1 +1]
		//ZO : z is normalied [0 +1]
		 mProjection = glm::perspectiveRH_ZO(glm::radians(fov), 800.f /600.f, near, far);
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
			if (Yaw > 89.0f)
				Yaw = 89.0f;
			if (Yaw < 1.f)
				Yaw = 1.f;
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
};


struct Controller
{
	int32 lastPosMouseX, lastPosMouseZ;
};

char map[18 * 18] = {
	'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
	'#','9','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','8','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','7','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','6','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','7','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','4','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','3','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','2','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
	'#','1','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',
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
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 18; ++x)
		{
			Tile ent = {
							map[x + y * 18]
							, x 
							, 17-y 
							, ((x>3 && map[x + y * 18] == '+') ? ((x-3)*0.6) : 0)
							, 0.f
							, 1.f
							,sf::Sprite(texture, font.getGlyph(map[x + y * 18], 24, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250, 205, 195));
			ent.sprite.setOrigin(11, 11);
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , float z, Camera& cam , float& scale, bool& render)
{
	glm::vec4 orig = { x, y, z , 1.f	 };
	glm::vec4 mFinal = cam.mProjection  * orig;
	mFinal /= mFinal.w;
	//std::cout << mFinal.z << std::endl;
	//std::cout << mFinal.x << " : " << mFinal.y << std::endl;
	mFinal = mFinal * 0.5f + 0.5f;
	//To do Z0(0,1) (NO par default) :  comment above and uncomment below
	//mFinal.x = mFinal.x * 0.5 + 0.5;
	//mFinal.y = mFinal.y * 0.5 + 0.5;

	mFinal.x =( mFinal.x * cam.viewport[2]) + cam.viewport[0];
	mFinal.y =(cam.viewport[3] - (mFinal.y * cam.viewport[3])) + cam.viewport[1];

	//mFinal.z = ((100.f - 0.1f) / 2.0f) * mFinal.z + ((100.f + 0.1f) / 2.0f);
	//std::cout << mFinal.z << std::endl;
	if (mFinal.z < -1|| mFinal.z > 1) //culling
	{
		render = false;
	}
	sf::Vector2f vec(mFinal.x , mFinal.y);
	//scale = 100.0 - (mFinal.z*100 / 2.0);
	
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
	camera.WorldUp = {0.f,0.f,1.f};
	camera.LastPosition = {-1,-1,-1};
	camera.Pitch = -90.f;
	camera.Yaw = 45.f;
	camera.Front = { 0.f,0.f,0.f };
	camera.MovementSpeed = SPEED;
	camera.MouseSensitivity = SENSITIVITY;
	camera.Zoom = ZOOM;
	// run the program as long as the window is open

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		bool moved = false;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				player.x -= 1.f;
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				player.x += 1.f;
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				player.y += 1.f;
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				player.y -= 1.f;
				moved = true;
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
				moved = true;
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				camera.ProcessMouseScroll(event.mouseWheelScroll.delta);
				moved = true;
			}
		}
		player.z = list.at( player.x + player.y * 18).z;
		camera.Position = { player.x , player.y  , player.z  };
		glm::vec3 target = { player.x , player.y , player.z };

		if (moved)
		{
			camera.updateCameraVectors(target);
		}
		
		window.clear(sf::Color::Black);
		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(400, 0);
		lines[1].position = sf::Vector2f(400, 600);

		window.draw(lines);

		lines[0].position = sf::Vector2f(0, 300);
		lines[1].position = sf::Vector2f(800, 300);
		window.draw(lines);
		//std::cout << camera.rotZ << std::endl;	
		bool render = true;
		int nb = 0;
		for (Tile ent : list)
		{
			render = true;
			scale = 1;
			ent.sprite.setPosition(to_global(ent.x,ent.y, ent.z,camera ,scale, render));
			if (!render) continue;
			ent.sprite.setRotation(camera.Pitch + 90.f);	
			ent.sprite.setScale(scale,scale);
			window.draw(ent.sprite);
			nb++;
		}
		//std::cout << nb + 1 << ":" << std::endl;
		player.sprite.setPosition(to_global(player.x, player.y, player.z, camera, scale, render));
		player.sprite.setRotation(camera.Pitch + 90.f);
		player.sprite.setScale(1, 1);
		window.draw(player.sprite);

		window.display();

	}

	return 0;
}