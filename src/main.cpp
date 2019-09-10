#include "Camera.cpp"
#include "timestep.cpp"

/*

GAME===>		  <===  Main   ===>	    <= Plateform
											=>Renderer		
=>GameState									=>Camera
=>Controller							
								
=>Cell
	=>Entity																					
														

Plateform must receive only glyph+pos+color and send controller input to main

*/


//TODO :
//		Culling
//		refactor
//		UI
//		FOV
//		Light
//      
//		Shader
//
//		Basic dungeon generation/basic 
//
//		Basic monster
//		Mouse picking AABB for each vertices = rotate the mouse pos by invert rectangle


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
				cell.block = true;
			}
			else
			{
				cell.glyph = '+';// y+48;
				cell.z = 0;//(x >= 3)? (x -3.f)*0.1 : 0.f;
			}
			cell.x = x;
			cell.y = y;
			cell.baseColor = sf::Color(200 - x + y * 18, 100 - x + y * 18, 200, 255);
			list.push_back(cell);
		}
	}

	list[6 + 6 * 18].glyph = '#';
	list[6 + 6 * 18].block = true;

	list[8 + 12 * 18].glyph = '#';
	list[8 + 12 * 18].block = true;
	return list;
}



void castLight(std::vector<Cell>& map, int row, int startX, int startY, float start, float end, int xx, int xy, int yx, int yy, float radius)
{
	float newStart = 0.0f;
	if (start < end) {
		return;
	}
	bool blocked = false;
	for (int distance = row; distance <= radius && !blocked; distance++) {
		int deltaY = -distance;
		for (int deltaX = -distance; deltaX <= 0; deltaX++) {
			int currentX = startX + deltaX * xx + deltaY * xy;
			int currentY = startY + deltaX * yx + deltaY * yy;
			float leftSlope = (deltaX - 0.5f) / (deltaY + 0.5f);
			float rightSlope = (deltaX + 0.5f) / (deltaY - 0.5f);

			if (!(currentX >= 0 && currentY >= 0 && currentX < 18 && currentY < 18) || start < rightSlope) {
				continue;
			}
			else if (end > leftSlope) {
				break;
			}

			//check if it's within the lightable area and light if needed
			if ((deltaX * deltaX) + (deltaY * deltaY) <
			((radius - 3) * (radius - 3))) {
				//float bright = (float)(1 - (rStrat.radius(deltaX, deltaY) / radius));
				map[currentX + currentY * 18].visible = true;
			}
		

			if (blocked) { //previous cell was a blocking one
				if (map[currentX + currentY * 18].block >= 1) {//hit a wall
					newStart = rightSlope;
					continue;
				}
				else {
					blocked = false;
					start = newStart;
				}
			}
			else {
				if (map[currentX + currentY * 18].block >= 1 && distance < radius) {//hit a wall within sight line
					blocked = true;
					castLight(map, distance + 1, startX, startY, start, leftSlope, xx, xy, yx, yy, radius);
					newStart = rightSlope;
				}
			}
		}
	}
}

//radius: max distance FOV;
void calculateFOV(std::vector<Cell>& map,  int startX, int startY, float radius)
{
	for (Cell& cell : map)
	{
		cell.visible = true;
	}

	map[startX + startY * 18].visible = true;
	
	//For each diagonals
	castLight(map, 1, startX , startY, 1.0f, 0.0f, 0, -1, 1, 0, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, -1, 0, 0, 1, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, 0, 1, 1, 0, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, 1, 0, 0, 1, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, 0, 1, -1, 0, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, 1, 0, 0, -1, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f, 0, -1, -1, 0, radius);
	castLight(map, 1, startX, startY, 1.0f, 0.0f,-1, 0, 0, -1, radius);
	
}


int main()
{
	GameConfig gc;

	sf::ContextSettings settings;
	//settings.depthBits = 0;
	//settings.stencilBits = 0;
	settings.antialiasingLevel =8;
	settings.majorVersion = 3;
	settings.minorVersion = 2;


	sf::RenderWindow  window(sf::VideoMode(gc.winWidth, gc.winHeight), "RL test", sf::Style::Default, settings);
	//sf::RenderTexture windowTexture;
	//windowTexture.create(gc.winWidth, gc.winHeight);

	window.setActive(true);
	float lastX = gc.winWidth / 2, lastY = gc.winHeight / 2;
	bool firstMouse = true;

	window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(60);


	sf::Font font;
	if (!font.loadFromFile(FONT_FILENAME))
	{
		// error...
	}


	sf::String fontText;
	for (int i = 31; i < 100; ++i)
	{
		fontText.insert(fontText.getSize(),(char)i);
	}
	std::cout << std::string(fontText) << std::endl;

	//Load Texture ASCII
	sf::Text fontTxt(fontText, font, 128);
	float width = fontTxt.findCharacterPos(35).x - fontTxt.findCharacterPos(34).x;
	//std::cout << std::string(fontText) <<  "\n" << width <<  "\n" << fontTxt.getCharacterSize() << std::endl;
	fontTxt.setOutlineThickness(3);
	fontTxt.setPosition(0, 0);

	sf::RenderTexture asciiTexture;
	asciiTexture.create(128*100, 138);
	asciiTexture.setSmooth(true);
	asciiTexture.draw(fontTxt);
	asciiTexture.display();
	asciiTexture.generateMipmap();

	sf::RenderTexture windowTexture;
	windowTexture.create(gc.winWidth, gc.winHeight);
	//windowTexture.setSmooth(true);
	
	// Check if shaders are available
	if (!sf::Shader::isAvailable())
	{
		std::cerr << "Shader are not available" << std::endl;
		return -1;
	}

	// Load shaders
	sf::Shader postEffect;
	if (!postEffect.loadFromFile(POST_SHADER_FILENAME, sf::Shader::Fragment))
	{
		std::cerr << "Error while loading shaders" << std::endl;
		return -1;
	}

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
	bool firstCam = true;
	glm::vec3 target;
	glm::vec3 targetDeplacement;
	GameTimer timer;
	timer.initialize();
	while (window.isOpen())
	{
		timer.startRenderFrame();
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
				gc.winWidth = event.size.width;
				gc.winHeight = event.size.height;

				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
				windowTexture.create(gc.winWidth, gc.winHeight);

				camera.viewport = { 0.0f, 0.0f, (float)gc.winWidth, (float)gc.winHeight };
				moved = true;
			}
		}

		player.z = map[player.x + player.y * 18].z;
		
		if (firstCam)
		{
			camera.Position = { player.x , player.y , player.z };
			target = { player.x , player.y , player.z };
			targetDeplacement = target;
			firstCam = false;
		}

		if (moved)
		{
			
			target = { player.x , player.y , player.z };	
		}

		map[player.x + player.y * 18].ent = &player;


		//wheighted avarege : v = ((v * (N - 1)) + w) / N; fast at start but decrease 
		camera.Position = ((camera.Position * (18.f -1.f)) + target) / 18.f;
		targetDeplacement = ((targetDeplacement * (18.f - 1.f)) + target) / 18.f;

		camera.updateCameraVectors(targetDeplacement);

		calculateFOV(map, player.x, player.y, 10);

		while (timer.doUpdate());

		//PRE-RENDERING
		camera.m_vertices.clear();
		camera.m_vertices.setPrimitiveType(sf::Quads);

		window.clear(sf::Color::Black);
		windowTexture.clear(sf::Color::Black);

		//Gen mapSprite(todo : based on fov) + (todo: light)
		std::vector<Glyph> glyphs;
		//GenGlyps
		for (Cell& cell : map)
		{ 
			Glyph g;
			if(cell.visible == true)
			{
				if (cell.ent != nullptr)
				{
					g = camera.to_global(cell.ent->x, cell.ent->y, cell.ent->z, cell.ent->glyph, cell.ent->baseColor, font);
					if (g.orig.z != -1) glyphs.push_back(g);
				}
				else
				{
					g = camera.to_global(cell.x, cell.y, cell.z, cell.glyph, cell.baseColor, font);
					if (g.orig.z != -1) glyphs.push_back(g);
					if (cell.glyph == '#')
					{
						g = camera.to_global(cell.x, cell.y, cell.z + 0.4, cell.glyph, cell.baseColor, font);
						if (g.orig.z != -1) glyphs.push_back(g);
						g = camera.to_global(cell.x, cell.y, cell.z + 0.8, cell.glyph, cell.baseColor, font);
						if (g.orig.z != -1) glyphs.push_back(g);
					}
				}
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
		std::cout <<timer.getFPS() << std::endl;

		
		windowTexture.draw(camera.m_vertices, &asciiTexture.getTexture());  //Draw all the ascii sprites

		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(gc.winWidth/2, 0);
		lines[1].position = sf::Vector2f(gc.winWidth/2, gc.winHeight);

		windowTexture.draw(lines);

		lines[0].position = sf::Vector2f(0, gc.winHeight / 2);
		lines[1].position = sf::Vector2f(gc.winWidth, gc.winHeight / 2);
		windowTexture.draw(lines);


		//sf::Sprite sprite(asciiTexture.getTexture());
		//windowTexture.draw(sprite);
		windowTexture.display();
		
		//draw to windows
		sf::Sprite endWindow(windowTexture.getTexture());
		postEffect.setUniform("texture", sf::Shader::CurrentTexture);
		window.draw(endWindow,&postEffect);

		window.display();

		//UI

		timer.endRenderFrame();
	}

	return 0;
}