#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>

#include <iostream>

# define M_PI           3.14159265358979323846

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


struct Tile
{
	char glyph;

	int32 x, y, z;
	int32 color;
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
	int32 x, y;
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
							, ((x>3 ) ? x-3 : 0)
							, 0
							,sf::Sprite(texture, font.getGlyph(map[x + y * 12], 24, false).textureRect )
			};
			ent.sprite.setColor(sf::Color(250, 205, 195));
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , Camera cam)
{
	 x = x * 24  - cam.x ;
	 y = y * 24  - cam.y ;

	float xp = (x ) * cosf(cam.rotZ) - (y ) * sinf(cam.rotZ);
	float yp = (x ) * sinf(cam.rotZ) + (y ) * cosf(cam.rotZ);

	sf::Vector2f vec(xp +(800/2), yp +(600/2));
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

	const sf::Texture& texture = font.getTexture(24);
	std::vector<Tile> list = gensprite_map(font, texture, map);
	Controller control = {};
	
	Player player =
	{
		'@'
		, 2 
		, 6 
		, 0
		, 0
		,sf::Sprite(texture, font.getGlyph('@', 24, false).textureRect)
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
		}
		int playerRx = player.x * 24;
		int playerRy = player.y * 24;
		camera = { playerRx + (10) , playerRy + (10), camera.rotZ };

		window.clear(sf::Color::Black);
		std::cout << camera.rotZ << std::endl;
		for (Tile ent : list)
		{
			ent.sprite.setPosition(to_global(ent.x,ent.y,camera));
			ent.sprite.setRotation(camera.rotZ * (180.f/M_PI));
			
			window.draw(ent.sprite);
		}
		std::cout << camera.x << ":"  << camera.y<< std::endl;
		player.sprite.setPosition(to_global(player.x, player.y, camera));
		player.sprite.setRotation(camera.rotZ * (180.f / M_PI));
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}