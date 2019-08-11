#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <algorithm>

#include "Camera.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.5f;
const float ZOOM = 10.0f;

//TODO :	
//		resize : struct GameConfig(taille ecran, global state)
//      light attenuation
//      
//		Control angle of view
//		Timestep
//      LinTerp for camera movement

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

struct Tile
{
	bool render;

	char glyph;

	float x, y, z;
	float winX, winY, winZ;
	int32 color;
	sf::Sprite sprite;

};



typedef Tile Player;


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
							true
							,map[x + y * 18]
							, x 
							, 17.f-y 
							, ((x>3 && map[x + y * 18] == '+') ? ((x-3)*0.6) : 0)
							, 0.f
							,0.f
							,0.f
							, 1.f
							,sf::Sprite(texture, font.getGlyph(map[x + y * 18], 48, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250- x + y * 18, 205 - x + y * 18, 195 - x + y * 18));
			ent.sprite.setOrigin(20, 20);
			list_entities.push_back(ent);
		}
	}
	return list_entities;
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

	const sf::Texture& texture = font.getTexture(48 );
	std::vector<Tile> list = gensprite_map(font, texture, map);
	Controller control = {};
	
	Player player =
	{
		true
		,'@'
		, 2.f 
		, 6.f 
		, 0.f
		, 0.f
		,0.f
		,0.f
		,0.f
		,sf::Sprite(texture, font.getGlyph('@', 48 , false).textureRect)
	};
	player.sprite.setOrigin(20, 20);
	Camera camera = { };
	camera.LastPosition = {-1,-1,-1};
	camera.Pitch = -90.f;
	camera.Yaw = 45.f;
	camera.MovementSpeed = SPEED;
	camera.MouseSensitivity = SENSITIVITY;
	camera.Zoom = ZOOM;
	// run the program as long as the window is open

	bool moved = true;
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

		player.z = 0;
		glm::vec3 target = { player.x , player.y , player.z };
		if (moved)
		{
			camera.Position = { player.x , player.y  , player.z };
			camera.updateCameraVectors(target);
			moved = false;
		}

		//PRE-RENDERING
		window.clear(sf::Color::Black);
		//Z-sorting
		for (Tile& ent : list)
		{ 
			camera.to_global(ent.x, ent.y, ent.z, ent.winX, ent.winY, ent.winZ, ent.render);
		}
		std::sort(list.begin(), list.end(),	[](const Tile& a, const Tile& b) {return a.winZ < b.winZ; });

		//RENDERING
		
		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(400, 0);
		lines[1].position = sf::Vector2f(400, 600);

		window.draw(lines);

		lines[0].position = sf::Vector2f(0, 300);
		lines[1].position = sf::Vector2f(800, 300);
		window.draw(lines);

		for (Tile& ent : list)
		{
			if (!ent.render) 
				continue;
			ent.sprite.setPosition(sf::Vector2f(ent.winX,ent.winY));
			ent.sprite.setRotation(camera.Pitch + 180.f);	
			ent.sprite.setScale(ent.winZ,ent.winZ);
			window.draw(ent.sprite);
		}
		camera.to_global(player.x, player.y, player.z,player.winX , player.winY ,player.winZ, player.render);
		player.sprite.setPosition(sf::Vector2f(player.winX, player.winY));
		player.sprite.setRotation(camera.Pitch + 180.f);
	
		player.sprite.setScale(player.winZ, player.winZ);
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}