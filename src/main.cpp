#include "Camera.cpp"

struct GameConfig
{
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.5f;
	const float ZOOM = 10.0f;

	int winWidth = 1280;
	int winHeight = 760;

};


struct Glyph
{
	bool render;
	float x, y, z;
	float winX, winY, winZ;

	int32 colorMod;
	sf::Sprite sprite;
};

struct Entity
{
	char glyph;
	float x, y, z;

	int32 baseColor;
};


struct Cell
{
	char glyph;
	float x, y, z;

	int32 baseColor;
	Entity* ent = nullptr;

};

//TODO :
//		FOV
//		Light
//      
//		Timestep
//      LinTerp for camera movement
//		LinLerp for animation
//		vertex array for sprite bashing

//"angle" of the keypress, subtract the camera Z rotation, find the nearest matching direction


struct Controller
{
	int32 lastPosMouseX, lastPosMouseZ;
};


std::vector<Cell> genRectangleRoom()
{
	std::vector<Cell> list;
	for (int y = 0; y <18; ++y)
	{
		for (int x = 0; x < 18; ++x)
		{
			Cell cell;
			if ((x == 0 || x == 17) || (y == 0 || y == 17))
			{
				cell.glyph = '#';
				cell.z = 0;
			}
			else
			{
				cell.glyph = y+48;
				cell.z = (x >= 3)? (x -3)*0.5 : 0.f;
			}
			list.push_back(cell);
		}
	}
	return list;
}


std::vector<Glyph> gensprite_map( sf::Font& font,const sf::Texture& texture ,std::vector<Cell>& vec)
{
	std::vector<Glyph> list_entities;
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 18; ++x)
		{
			if (vec[x + y * 18].ent == nullptr)
			{
				Glyph ent = {
							true
							, x
							, y
							, vec[x + y * 18].z
							,0.f
							,0.f
							,0.f
							,0.f
							,sf::Sprite(texture, font.getGlyph(vec[x + y * 18].glyph, 48, false).textureRect)
				};
				ent.sprite.setColor(sf::Color(200 - x + y * 18, 100 - x + y * 18, 200));
				ent.sprite.setOrigin(20, 20);
				list_entities.push_back(ent);
			}
			else if(vec[x + y * 18].ent != nullptr)
			{
				Glyph ent = {
							true
							, x
							, y
							, vec[x + y * 18].ent->z
							,0.f
							,0.f
							,0.f
							,0.f
							,sf::Sprite(texture, font.getGlyph(vec[x + y * 18].ent->glyph, 48, false).textureRect)
				};
				ent.sprite.setColor(sf::Color(250 - x + y * 18, 205 - x + y * 18, 195 - x + y * 18));
				ent.sprite.setOrigin(20, 20);
				list_entities.push_back(ent);
			}		
		}
	}
	return list_entities;
}

int main()
{
	GameConfig gc;

	sf::RenderWindow  window(sf::VideoMode(gc.winWidth, gc.winHeight), "RL test");
	float lastX = gc.winWidth / 2, lastY = gc.winHeight / 2;
	bool firstMouse = true;

	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);


	sf::Font font;
	if (!font.loadFromFile("nasalization-rg.ttf"))
	{
		// error...
	}

	const sf::Texture& texture = font.getTexture(48 );


	//MAP
	std::vector<Cell> map = genRectangleRoom();
	Controller control = {};
	
	Entity player =
	{
		'@'
		,0.f
		,0.f
		,0.f
		,0.f
	};

	map[3 + 3 * 18].ent = &player;

	player.x = 3;
	player.y = 3;

	Camera camera = { };
	camera.LastPosition = { player.x,player.y,player.z };
	camera.Pitch =  -90.f;
	camera.Yaw = 45.f;
	camera.MovementSpeed = gc.SPEED;
	camera.MouseSensitivity = gc.SENSITIVITY;
	camera.Zoom = gc.ZOOM;
	camera.viewport = {0.0f, 0.0f, (float)gc.winWidth , (float)gc.winHeight};
	// run the program as long as the window is open

	bool moved = true;
	std::vector<Glyph> glyphs;
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
				map[player.x + player.y * 18].ent = nullptr;
				player.y += glm::round(glm::sin(glm::radians(camera.Pitch + 270)));
				player.x += glm::round(glm::cos(glm::radians(camera.Pitch + 270)));
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				map[player.x + player.y * 18].ent = nullptr;
				player.y += glm::round(glm::sin(glm::radians(camera.Pitch + 90)));
				player.x += glm::round(glm::cos(glm::radians(camera.Pitch + 90)));
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				map[player.x + player.y * 18].ent = nullptr;
				player.y += glm::round(glm::sin(glm::radians(camera.Pitch+180)));
				player.x += glm::round(glm::cos(glm::radians(camera.Pitch+180)));
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				map[player.x + player.y * 18].ent = nullptr;
				player.y += glm::round(glm::sin(glm::radians(camera.Pitch )));
				player.x += glm::round(glm::cos(glm::radians(camera.Pitch )));
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
			if (event.type == sf::Event::Resized)
			{
				
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));

				gc.winWidth = event.size.width;
				gc.winHeight = event.size.height;

				camera.viewport = { 0.0f, 0.0f, (float)gc.winWidth, (float)gc.winHeight };
				moved = true;
			}
		}

		player.z = map[player.x + player.y * 18].z;
		glm::vec3 target = { player.x , player.y , player.z };
		if (moved)
		{
			map[player.x + player.y * 18].ent = &player;
			camera.Position = { player.x , player.y  , player.z };
			camera.updateCameraVectors(target);
	
			moved = false;
		}

		//PRE-RENDERING
		window.clear(sf::Color::Black);
		//Gen mapSprite(todo : based on fov) + (todo: light)
		glyphs = gensprite_map(font, texture, map);
		//Z-sorting
		for (Glyph& glyph : glyphs)
		{ 
			camera.to_global(glyph.x, glyph.y, glyph.z, glyph.winX, glyph.winY, glyph.winZ, glyph.render);
		}
		std::sort(glyphs.begin(), glyphs.end(),	[](const Glyph& a, const Glyph& b) {return a.winZ < b.winZ; });

		//RENDERING
		for (Glyph& ent : glyphs)
		{
			if (!ent.render) 
				continue;
			ent.sprite.setPosition(sf::Vector2f(ent.winX,ent.winY));
			ent.sprite.setRotation(camera.Pitch + 90.f);	
			ent.sprite.setScale(ent.winZ,ent.winZ);
			window.draw(ent.sprite);
		}
	
		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(gc.winWidth/2, 0);
		lines[1].position = sf::Vector2f(gc.winWidth/2, gc.winHeight);

		window.draw(lines);

		lines[0].position = sf::Vector2f(0, gc.winHeight / 2);
		lines[1].position = sf::Vector2f(gc.winWidth, gc.winHeight / 2);
		window.draw(lines);
		window.display();
	}

	return 0;
}