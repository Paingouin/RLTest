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




struct Entity
{
	char glyph;
	float x, y, z;

	sf::Color baseColor;
};


struct Cell
{
	char glyph;
	float x, y, z;

	sf::Color baseColor;
	Entity* ent = nullptr;

};

//TODO :
//		FOV
//		Light
//      
//		Timestep
//      LinTerp for camera movement
//		LinLerp for animation

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
				cell.glyph = '+';// y+48;
				cell.z = (x >= 3)? (x -3)*0.5 : 0.f;
			}
			cell.x = x;
			cell.y = y;
			cell.baseColor = sf::Color(200 - x + y * 18, 100 - x + y * 18, 200, 255);
			list.push_back(cell);
		}
	}
	return list;
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

	const sf::Texture& texture = font.getTexture(48);


	//MAP
	std::vector<Cell> map = genRectangleRoom();
	Controller control = {};
	
	Entity player =
	{
		'@'
		,0.f
		,0.f
		,0.f
		,sf::Color(255, 255, 255, 255)
	};

	map[3 + 3 * 18].ent = &player;

	player.x = 3;
	player.y = 3;

	Camera camera = { };
	camera.LastPosition = { player.x,player.y,player.z };
	camera.Pitch =  -90.f;
	camera.Yaw = 0.1f;
	camera.MovementSpeed = gc.SPEED;
	camera.MouseSensitivity = gc.SENSITIVITY;
	camera.Zoom = gc.ZOOM;
	camera.viewport = {0.0f, 0.0f, (float)gc.winWidth , (float)gc.winHeight};
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
		camera.m_vertices.clear();
		camera.m_vertices.setPrimitiveType(sf::Quads);
		window.clear(sf::Color::Black);
		//Gen mapSprite(todo : based on fov) + (todo: light)
		std::vector<Glyph> glyphs;
		//GenGlyps
		for (Cell& cell : map)
		{ 
			Glyph g;
			if (cell.ent != nullptr)
			{
				g=camera.to_global(cell.ent->x, cell.ent->y, cell.ent->z, cell.ent->glyph, cell.ent->baseColor, font);
				if (g.orig.z != -1) glyphs.push_back(g);
			}
			else
			{
				g = camera.to_global(cell.x, cell.y, cell.z, cell.glyph, cell.baseColor, font);
				if (g.orig.z != -1) glyphs.push_back(g);
			}
		}
		
		//Z-sorting
		std::sort(glyphs.begin(), glyphs.end(),	[](const Glyph& a, const Glyph& b) {return a.orig.z > b.orig.z; });
		for (Glyph& glyph : glyphs)
		{
			camera.m_vertices.append(glyph.vertices[0]);
			camera.m_vertices.append(glyph.vertices[1]);
			camera.m_vertices.append(glyph.vertices[2]);
			camera.m_vertices.append(glyph.vertices[3]);

		}

		sf::RenderStates states;
		states.texture = &font.getTexture(128);
		window.draw(camera.m_vertices,states );
	
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