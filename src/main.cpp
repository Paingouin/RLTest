#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>


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
	int x, y;
	double rotZ;
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
			ent.sprite.setColor(sf::Color(255, 255, 255));
			list_entities.push_back(ent);
		}
	}
	return list_entities;
}

sf::Vector2f to_global(float x, float y , Camera cam)
{
	 x = x * 24 ;
	 y = y * 24 ;

	float xp = (x - cam.x) * cosf(cam.rotZ) - (y - cam.y) * sinf(cam.rotZ);
	float yp = (x - cam.x) * sinf(cam.rotZ) + (y - cam.y) * cosf(cam.rotZ);

	sf::Vector2f vec( xp , yp );
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

	
	Player player =
	{
		'@'
		, 2 
		, 6 
		, 0
		, 0
		,sf::Sprite(texture, font.getGlyph('@', 24, false).textureRect)
	};
	Camera camera = { 800/4, 600/4 };

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
				camera.rotZ = event.mouseMove.x /600.0f;
			}
		}

		camera = {( player.x * 24) - (800/2), player.y * 24 -(600/2), camera.rotZ };

		window.clear(sf::Color::Black);

		for (Tile ent : list)
		{
			ent.sprite.setPosition(to_global(ent.x,ent.y,camera));
			window.draw(ent.sprite);
		}

		player.sprite.setPosition(800/2, 600/2);
		window.draw(player.sprite);

		window.display();
	}

	return 0;
}