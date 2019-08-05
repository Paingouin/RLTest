#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>


# define M_PI           3.14159265358979323846

int PERSPECTIVE = 800 * 0.8;


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
	int32 x, y,z;
	double rotZ;
};


struct Controller
{
	int32 lastPosMouse;
};

char map[12 * 12] = {
	'#','#','#','#','#','#','#','#','#','#','#','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','+','+','+','+','+','+','+','+','+','+','#',
	'#','#','#','#','#','#','#','#','#','#','#','#',
};



std::vector<Tile> gensprite_map( sf::Font& font,const sf::Texture& texture ,char* map)
{
	std::vector<Tile> list_entities;
	for (int y = 0; y < 12; ++y)
	{
		for (int x = 0; x < 12; ++x)
		{
			Tile ent = {
							map[x + y * 12]
							, x 
							, y 
							, ((x>3 && map[x + y * 12] == '+') ? x-3 : 0)
							, 0
							, 1
							,sf::Sprite(texture, font.getGlyph(map[x + y * 12], 24, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250, 205, 195));
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , float z, Camera cam ,double &scale)
{
	// x = x * 24  - cam.x ;
	// y = y * 24  - cam.y ;

	float fov = 45;
	double S = 1 / (glm::tan((fov / 2) * (M_PI / 180)));
	double near = 0.1;
	double far = 100;

	glm::mat4 mPerspective =
	{
		 S , 0 , 0 , 0,
		 0 , S , 0 , 0,
		 0 , 0 , -(far /(far - near)) ,-1 ,
		 0 , 0 , -((far * near) / (far - near)) , 0
	};

	glm::mat4 mTranslate =
	{
		 1 , 0 , 0 , 0,
		 0 , 1 , 0 , 0,
		 0 , 0 , 1 , 0,
		 -cam.x , -cam.y , -cam.z, 1
	 };

	 glm::mat4 mRotation =
	{
		cosf(cam.rotZ) , sinf(cam.rotZ) ,  0, 0,
		-sinf(cam.rotZ) ,  cosf(cam.rotZ),  0 , 0,
		 0, 0, 1,0,
		 0 , 0,   0, 1
	};

	glm::mat4 mScale =
	{
		 24, 0  , 0,  0,
		 0 , 24, 0,  0,
		 0 , 0 ,  24  , 0,
		 0 , 0,   0,  1
	};

	glm::vec4 orig = { x, y, z,1 };
	
	auto model =/* mRotation **/ mTranslate * mScale ;

	auto camPos = glm::vec3(cam.x, cam.z, cam.z) * 23;
	glm::mat4 mView = glm::lookAt(
		glm::vec3(cam.x, cam.z, cam.z), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	auto mProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f) ;


	auto MVPmatric = mProjection * mView * model;
	auto mFinal = MVPmatric * orig;

	//float xp = view.x * cosf(cam.rotZ) - (view.y ) * sinf(cam.rotZ);
	//float yp = (view.x ) * sinf(cam.rotZ) + (view.y ) * cosf(cam.rotZ);

	sf::Vector2f vec(mFinal.x +(800/2) , mFinal.y  +(600/2));	
	return vec;
}

int main()
{
	sf::RenderWindow  window(sf::VideoMode(800, 600), "RL test");

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
	Camera camera = {  };

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

				camera.rotZ += (control.lastPosMouse - event.mouseMove.x) /100.0f;
				control.lastPosMouse = event.mouseMove.x;
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				//std::cout << "wheel movement: " <<  << std::endl
					scale = glm::mix(scale, scale + (event.mouseWheelScroll.delta), 0.1);
			}
		}
		camera = { player.x*24 , player.y*24 , 30, camera.rotZ };

		window.clear(sf::Color::Black);
		std::cout << camera.rotZ << std::endl;
		double scaleProject = 1;
		for (Tile ent : list)
		{
			ent.sprite.setPosition(to_global(ent.x,ent.y, ent.z,camera, scaleProject));
			ent.sprite.setRotation(camera.rotZ * (180.f/M_PI));
			ent.sprite.setScale(1, 1);
			window.draw(ent.sprite);
		}
		std::cout << camera.x << ":"  << camera.y<< std::endl;
		player.sprite.setPosition(to_global(player.x, player.y,player.z, camera, scaleProject));
		player.sprite.setRotation(camera.rotZ * (180.f / M_PI));
		player.sprite.setScale(1, 1);
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}