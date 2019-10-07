#include "Camera.cpp"
#include "timestep.cpp"

/*

One main per plateform.

GAME===>		  <===  Main	 <= Plateform
										=>Renderer		
=>GameState								=>Camera
										=>Audio
								
=>Cell
	=>Entity																					
														

Plateform must receive only map(pos+color + state of effects) and send controller input to the game

*/


//TODO :
//		FOV
//		Light
//      
//		Shader
//
//		Basic dungeon generation/basic 
//
//		Item
//		Basic monster
//		Mouse picking :  check inside UI, if not , check inside rect for each glyph = rotate the mouse pos by invert rectangle


struct Controller
{
	int32 lastPosMouseX, lastPosMouseZ;
};

//radius: max distance FOV;
void calculateFOV(Map& map,  int startX, int startY, double radius)
{
	for (Cell& cell : map.cells)
	{
		cell.visible = false;
	}

	map.at(startX,startY).visible = true;
	
	////For each diagonals
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 0,-1, 1, 0,  radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f,-1, 0, 0, 1, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 0, 1, 1, 0, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 1, 0, 0, 1, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 0, 1, -1, 0, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 1, 0, 0, -1, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f, 0, -1, -1, 0, radius);
	map.castLight( 1, startX, startY, 1.0f, 0.0f,-1, 0, 0, -1, radius);
	
}

void calculateLight(Map& map, int startX, int startY, double radius, sf::Color color)
{
	////For each diagonal
	map.at(startX, startY).baseColor = color;
	map.at(startX, startY).lightLevel = 1 ;


	map.castLight(1, startX, startY, 1.0f, 0.0f, 0, -1, 1, 0, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, -1, 0, 0, 1, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, 0, 1, 1, 0, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, 1, 0, 0, 1, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, 0, 1, -1, 0, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, 1, 0, 0, -1, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, 0, -1, -1, 0, radius, color);
	map.castLight(1, startX, startY, 1.0f, 0.0f, -1, 0, 0, -1, radius, color);


	for (Cell& cell : map.cells)
	{
		if (cell.colorToAdd == sf::Color::Black  )
			continue;
		glm::aligned_vec3 col = { cell.colorToAdd.r, cell.colorToAdd.g, cell.colorToAdd.b };
		glm::aligned_vec3 baseCol = { cell.baseColor.r, cell.baseColor.g, cell.baseColor.b };

		//col = col *  ;
		
		col.x = col.x + (baseCol.x - col.x) * (1 - cell.lightLevel);
		col.y = col.y + (baseCol.y - col.y) * (1 - cell.lightLevel);
		col.z = col.z + (baseCol.z - col.z) * (1 - cell.lightLevel);

		if (cell.lightLevel != 1)
			cell.baseColor = sf::Color(col.x, col.y, col.z);

		cell.colorToAdd = sf::Color::Black;
	}
}

int main()
{
	GameConfig gc;

	sf::ContextSettings settings;
	//settings.depthBits = 0;
	//settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 3;
	settings.minorVersion = 2;


	sf::RenderWindow  window(sf::VideoMode(gc.winWidth, gc.winHeight), "RL test", sf::Style::Default, settings);
	//sf::RenderTexture windowTexture;
	//windowTexture.create(gc.winWidth, gc.winHeight);

	window.setActive(true);
	float lastX = gc.winWidth / 2, lastY = gc.winHeight / 2;
	bool firstMouse = true;

	//window.setVerticalSyncEnabled(true);
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
	float width = fontTxt.findCharacterPos(25).x - fontTxt.findCharacterPos(34).x;
	//std::cout << std::string(fontText) <<  "\n" << width <<  "\n" << fontTxt.getCharacterSize() << std::endl;
	fontTxt.setOutlineThickness(3);
	fontTxt.setPosition(0, 0);

	sf::RenderTexture asciiTexture;
	asciiTexture.create(128*100, 138);//NOTE : Disable this to see the sprites
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

	// Load shaders"'
	sf::Shader postEffect;
	if (!postEffect.loadFromFile(POST_SHADER_FILENAME, sf::Shader::Fragment))
	{
		std::cerr << "Error while loading shaders" << std::endl;
		return -1;
	}

	//UI (and controls? )
	UI ui = {};

	//MAP
	Map map;
	map.genRectangleRoom(30,30);

	//Control
	Controller control = {};
	
	Entity player =
	{
		'@'
		,0.f
		,0.f
		,0.f
		,sf::Color(255, 255, 255, 255)
	};

	player.light = new LightSource;
	player.light->radius = 8;
	player.light->color = sf::Color(242, 204, 133);


	map.at(3,3).ent = &player;

	player.x = 3;
	player.y = 3;

	Camera camera = { };
	camera.LastPosition = { player.x,player.y,player.z };
	camera.Pitch =  -90.f;
	camera.Yaw = 0.1f;
	camera.MovementSpeed = gc.SPEED;
	camera.MouseSensitivity = gc.SENSITIVITY;
	camera.Zoom = gc.ZOOM;
	camera.viewport = {0.0f, 0.0f, (double)gc.winWidth , (double)gc.winHeight};
	// run the program as long as the window is open


	bool moved = true;
	bool firstCam = true;
	glm::aligned_vec3 target;
	glm::aligned_vec3 targetDeplacement;

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
				
				int deltaY = glm::round(glm::sin(glm::radians(camera.Pitch + 270)));
				int deltaX = glm::round(glm::cos(glm::radians(camera.Pitch + 270)));
				map.moveEntity(player, deltaX, deltaY);
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				int deltaY = glm::round(glm::sin(glm::radians(camera.Pitch + 90)));
				int deltaX = glm::round(glm::cos(glm::radians(camera.Pitch + 90)));
				map.moveEntity(player, deltaX, deltaY);
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				int deltaY = glm::round(glm::sin(glm::radians(camera.Pitch+180)));
				int deltaX = glm::round(glm::cos(glm::radians(camera.Pitch+180)));
				map.moveEntity(player, deltaX, deltaY);
				moved = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				int deltaY = glm::round(glm::sin(glm::radians(camera.Pitch )));
				int deltaX = glm::round(glm::cos(glm::radians(camera.Pitch )));
				map.moveEntity(player, deltaX, deltaY);
				moved = true;
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (ui.mouseDown == 0)
						ui.mouseDown = 1;
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (ui.mouseDown == 1)
						ui.mouseDown = 0;
				}
			}

			if (event.type == sf::Event::MouseMoved && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
			{

				if (firstMouse)
				{
					lastX = event.mouseMove.x;
					lastY = event.mouseMove.y;
					firstMouse = false;
				}

				double xoffset = event.mouseMove.x - lastX;
				double yoffset = lastY - event.mouseMove.y; // reversed since y-coordinates go from bottom to top

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

				camera.viewport = { 0.0f, 0.0f, (double)gc.winWidth, (double)gc.winHeight };
				moved = true;
			}
		}

		player.z = map.at(player.x, player.y).z +0.3;
		
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

		//wheighted avarege : v = ((v * (N - 1)) + w) / N; fast at start but decrease 
		camera.Position = ((camera.Position * (17.f -1.f)) + target) / 17.f;
		targetDeplacement = ((targetDeplacement * (17.f - 1.f)) + target) / 17.f;

		camera.updateCameraVectors(targetDeplacement);

		//DO FOV
		calculateFOV(map, player.x, player.y, 40);
		//DO LIGHT
		for (Cell& cell : map.cells)
		{
			cell.baseColor = sf::Color::Black;
			cell.colorToAdd = sf::Color::Black;
			cell.lightLevel = 0;
		}


		for (Cell& cell : map.cells)
		{
			if (cell.light != nullptr)
			{
				calculateLight(map, cell.x, cell.y, cell.light->radius, cell.light->color);
			}
		}


		if (player.light != nullptr)
			calculateLight(map, player.x, player.y, player.light->radius, player.light->color);

		//END LIGHT


		while (timer.doUpdate());

		//PRE-RENDERING
		camera.m_vertices.clear();
		camera.m_vertices.setPrimitiveType(sf::Quads);

		window.clear(sf::Color::Black);
		windowTexture.clear(sf::Color::Black);

		std::vector<Glyph> glyphs = camera.to_global(map.cells, player.z);
		
		//Z-sorting
		std::sort(glyphs.begin(), glyphs.end(),	[](const Glyph& a, const Glyph& b) {return a.orig.z > b.orig.z; });

		camera.m_vertices.resize(glyphs.size()*4);
		int i = 0;
		for (Glyph& glyph : glyphs)
		{
			camera.m_vertices[i++] = glyph.vertices[0];
			camera.m_vertices[i++] = glyph.vertices[1];
			camera.m_vertices[i++] = glyph.vertices[2];
			camera.m_vertices[i++] = glyph.vertices[3];
		}

		windowTexture.draw(camera.m_vertices, &asciiTexture.getTexture());  //Draw all the ascii sprites

		/*sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(gc.winWidth/2, 0);
		lines[1].position = sf::Vector2f(gc.winWidth/2, gc.winHeight);

		windowTexture.draw(lines);

		lines[0].position = sf::Vector2f(0, gc.winHeight / 2);
		lines[1].position = sf::Vector2f(gc.winWidth, gc.winHeight / 2);
		windowTexture.draw(lines);*/



		//UI
		ui.prepare();
		timer.endRenderFrame();
		sf::Text fpsTxt("Avg FPS : " + std::to_string(timer.getFPS())
					+ "\nFPS: " + std::to_string(timer.getLastFrameTime())
					+ "\nTimeForCalc: " + std::to_string(timer.getLastUpdateTime())
					+ "\nNb Sprites : " + std::to_string(glyphs.size())
					+ "\nPos Player X:" + std::to_string((int)player.x) + " Y:" + std::to_string((int)player.y)
					, font, 16);
		ui.drawRect(0, 0, 100, 50, sf::Color(0xff << (ui.mouseDown * 8) ), fpsTxt, windowTexture);

		ui.finish();


		//sf::Sprite sprite(asciiTexture.getTexture());

		//windowTexture.draw(sprite);
		windowTexture.display();
		
		//draw to windows
		sf::Sprite endWindow(windowTexture.getTexture());
		postEffect.setUniform("texture", sf::Shader::CurrentTexture);
		window.draw(endWindow,&postEffect);

		window.display();


		//TIMER
		timer.sleepAfterRender();//to force the 60fps when vsync
		timer.endafterSleep();
	}

	return 0;
}