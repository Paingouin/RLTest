#include "main.h"

struct Camera
{
	// Camera Attributes
	glm::aligned_vec3 Position;
	glm::aligned_vec3 LastPosition;

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float fov = 30.0f;
	//double S = 1.0 / (glm::tan(fov / 2.0));
	float near = 0.1f;
	float far = 1000.f;
	float heightOfNearPlane;

	glm::aligned_vec4 viewport;
	glm::aligned_vec4 camRight;
	glm::aligned_vec4 camUp;
	glm::aligned_vec3 camFront;
	glm::aligned_mat4x4 mModel;
	glm::aligned_mat4x4 mRotate;
	glm::aligned_mat4x4 mView;
	glm::aligned_mat4x4 mProjection;
	glm::aligned_mat4x4 mModelView;
	glm::aligned_mat4x4 mTotal;

	glm::aligned_vec3 posOffset;

	sf::VertexArray m_vertices;

	float s = glm::sin(glm::radians(Pitch));
	float c = glm::cos(glm::radians(Pitch));



	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors(glm::aligned_vec3 target)
	{
		// Calculate the new position of the camera
		
		posOffset.x = Zoom * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		posOffset.y = Zoom * sin(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		posOffset.z = Zoom * cos(glm::radians(Yaw));

		glm::aligned_mat4x4 mScale =
		{
			 1.f , 0.f, 0.f, 0.f,
			 0.f , 1.f, 0.f, 0.f,
			 0.f , 0.f, 1.f, 0.f,
			 0.f , 0.f, 0.f, 1.f
		};
		glm::aligned_mat4x4 mTranslate =
		{
			 1.f , 0.f, 0.f, 0.0f,//X
			 0.f , 1.f, 0.f, 0.0f,//Y
			 0.f , 0.f, 1.f, 0.0f,//Z
			 0.f , 0.f, 0.f, 1.f
		};

	 //   mRotate =
		//{
		//	 glm::cos(glm::radians(Pitch)), -glm::sin(glm::radians(Pitch)) , 0.f, 0.0f,//X
		//	 glm::sin(glm::radians(Pitch)), glm::cos(glm::radians(Pitch)) , 0.f, 0.0f,//Y
		//	 0.f , 0.f, 1.f, 0.0f,//Z
		//	 0.f , 0.f, 0.f, 1.f
		//};
		//https://gamedev.stackexchange.com/questions/54391/scaling-point-sprites-with-distance/65770#65770
		/*heightOfNearPlane = ((float)abs(viewport[3] - viewport[1]) /
			(2 * tan(0.5 * fov * glm::pi<float>() / 180.0)));*/

		// calculate the model matrix for each object and pass  before drawing
		 //model = glm::dmat4(1.0f); // make sure to initialize matrix to identity matrix first;
	 //translation*rotation*scale
	
		glm::aligned_vec4 camPos = {(float) Position.x +posOffset.x , (float)Position.y + posOffset.y,(float)Position.z + posOffset.z  , 1.f };
		glm::aligned_vec4 tarPos = { (float) target.x  , (float)target.y , (float)target.z  , 1.f };
		camFront = { 0.0f, 0.0f,1.f};

		//mRotate = glm::rotate(glm::radians(-Pitch - 180), camFront);
		mModel = mTranslate * mScale;
		glm::aligned_vec3 camPos3 = mModel * camPos;
		glm::aligned_vec3 tarPos3 = mModel * tarPos;
		//std::cout << "before cam:" << camPos3.x << " : " << camPos3.y << " : " << camPos3.z << std::endl;
		mView = glm::lookAtRH(
			camPos3, // Camera in World Space  
			tarPos3, // and looks at the target
			{ 0.f, 0.f, 1.f }// Head is up (set to 0,-1,0 to look upside-down)
		);

		//camRight = { mView[0][0],  mView[1][0], mView[2][0],0.f };
		//camUp = { mView[0][1], mView[1][1], mView[2][1] , 0.f};

		camFront = glm::normalize(camPos3 - tarPos3);

		//NO : z is Normalized  [-1 +1]
		//ZO : z is normalied [0 +1]
		
		mProjection = glm::perspectiveRH_NO(glm::radians(fov), viewport[2] / viewport[3], near, far) ;
		//mProjection = glm::orthoRH_NO(viewport[0], viewport[1], viewport[], 0.f, 0.f, 1000.f);

		mTotal = mProjection * mView  * mModel;
		mModelView = mView * mModel; //now we can use the camera View space (so in2d from the camera)

		s = glm::sin(glm::radians(Pitch));
		c = glm::cos(glm::radians(Pitch));
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	inline void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//std::cout << xoffset << std::endl;
		Yaw += yoffset;
		Pitch -= xoffset;
		//Pitch = Pitch - 360 * floor(Pitch / 360);
		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Yaw > 90.0f)
				Yaw = 90.0f;
			if (Yaw < 0.1f)
				Yaw = 0.1f;
		}

	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	inline void ProcessMouseScroll(float yoffset)
	{
		Zoom -= yoffset;


		if (Zoom >= 1.0f && Zoom <= 90.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 90.0f)
			Zoom = 90.0f;
	}

	void  __fastcall spriteFromCell(Glyphs& glyphs,Cell& cell, double heightMod = 0.f , float playerHeight =0.0f)
	{
		glm::aligned_vec4 orig;
		if (cell.ent == nullptr)
		{
			orig = { cell.x, cell.y, cell.z + heightMod, 1.f };
		}
		else
		{
			orig = { cell.ent->x, cell.ent->y, cell.ent->z + heightMod, 1.f };
		}
			
		glm::aligned_vec4 a[5];
		a[0] = mModelView * orig * 1.7f;  //POS : center
		a[1] = { -0.5,    0.5, a[0].z ,1.f }; //LU
		a[2] = { 0.5,     0.5, a[0].z ,1.f }; //RU
		a[3] = { 0.5,    -0.5, a[0].z ,1.f }; //RB
		a[4] = { -0.5,   -0.5, a[0].z ,1.f }; //LB

		if (cell.ent  == nullptr)
		{
			float tmpX = a[1].x;
			float tmpY = a[1].y;
			a[1].x = tmpX * c - tmpY * s;
			a[1].y = tmpX * s + tmpY * c;
			tmpX = a[2].x;
			tmpY = a[2].y;
			a[2].x = tmpX * c - tmpY * s;
			a[2].y = tmpX * s + tmpY * c;
			tmpX = a[3].x;
			tmpY = a[3].y;
			a[3].x = tmpX * c - tmpY * s;
			a[3].y = tmpX * s + tmpY * c;
			tmpX = a[4].x;
			tmpY = a[4].y;
			a[4].x = tmpX * c - tmpY * s;
			a[4].y = tmpX * s + tmpY * c;

		}

		a[1] += a[0];
		a[2] += a[0];
		a[3] += a[0];
		a[4] += a[0];


	//	glyphs.coordinates.insert(glyphs.coordinates.end(), a, a + (sizeof(a) / sizeof(a[0])));//below seems to be faster when profiling...
		glyphs.coordinates.emplace_back(a[0]);
		glyphs.coordinates.emplace_back(a[1]);
		glyphs.coordinates.emplace_back(a[2]);
		glyphs.coordinates.emplace_back(a[3]);
		glyphs.coordinates.emplace_back(a[4]);

		glyphs.cells.emplace_back(&cell);

		float delta = glm::abs(orig.z - playerHeight);

		glyphs.lightCorrection.emplace_back(glm::clamp(1.0 - (delta * delta) / (2.0 * 2.0), 0.0, 1.0)  );

	}


	//Calculate Global position of all cells to the screen based of camera
	std::vector<Glyph> __fastcall to_global(std::vector<Cell>& cells , float playerHeight)
	{
		Glyphs glyphs;
		glyphs.coordinates.reserve(cells.size() * 12);
		glyphs.cells.reserve(cells.size()* 12);

		s = glm::sin(glm::radians((180 - Pitch)));
		c = glm::cos(glm::radians((180 - Pitch)));


		for (Cell& cell : cells)
		{
			if (cell.visible == true)
			{
				spriteFromCell(glyphs, cell, 0.0f, playerHeight);
				if (cell.glyph == '#' && cell.ent == nullptr)
				{
					spriteFromCell(glyphs, cell,0.4f , playerHeight);
					spriteFromCell(glyphs, cell,0.8f , playerHeight);
				}
			}
		}

		for (glm::aligned_vec4& point : glyphs.coordinates)
		{
			point = mProjection * point;
		}

		for (glm::aligned_vec4& point : glyphs.coordinates)
		{
			point /= point.w;
		}


		std::vector<Glyph> returnedGlyph;
		returnedGlyph.reserve(glyphs.coordinates.size() / 5);
		
		for (int i = 0 ,cellCount = 0;  i < glyphs.coordinates.size(); )
		{
			glm::aligned_vec4 orig = glyphs.coordinates[i++];
			glm::aligned_vec4 LU = glyphs.coordinates[i++];
			glm::aligned_vec4 RU = glyphs.coordinates[i++];
			glm::aligned_vec4 RB = glyphs.coordinates[i++];
			glm::aligned_vec4 LB = glyphs.coordinates[i++];

			if (
				((LU.z > -1.f && LU.z < 1.f)
					&& (RU.z > -1.f && RU.z < 1.f)
						&& (RB.z > -1.f && RB.z < 1.f) 
							&& (LB.z > -1.f && LB.z < 1.f))
				 && 
						((LU.y > -1.f && LU.y < 1.f)
						&& (RU.y > -1.f && RU.y < 1.f)
						&& (RB.y > -1.f && RB.y < 1.f)
						&& (LB.y > -1.f && LB.y < 1.f))
				&&
						((LU.x > -1.f && LU.x < 1.f)
						&& (RU.x > -1.f && RU.x < 1.f)
						&& (RB.x > -1.f && RB.x < 1.f)
						&& (LB.x > -1.f && LB.x < 1.f))
				) //culling
			{

				LU.x = (LU.x * viewport[2]) + viewport[2] / 2;
				LU.y = (viewport[3] / 2 - (LU.y * viewport[3]));
				RU.x = (RU.x * viewport[2]) + viewport[2] / 2;
				RU.y = (viewport[3] / 2 - (RU.y * viewport[3]));
				RB.x = (RB.x * viewport[2]) + viewport[2] / 2;
				RB.y = (viewport[3] / 2 - (RB.y * viewport[3]));
				LB.x = (LB.x * viewport[2]) + viewport[2] / 2;
				LB.y = (viewport[3] / 2 - (LB.y * viewport[3]));

				//float distance = mFinal.w; // /!\ the distance is given by the w value, not the Z
				//orig.x /= orig.w;
				//orig.y /= orig.w;

				//orig /= orig.w;
				//scale = heightOfNearPlane * 0.015f / distance;
				//orig = orig * 0.5f + 0.5f;

				sf::Vertex quad1;
				sf::Vertex quad2;
				sf::Vertex quad3;
				sf::Vertex quad4;
				// define its 4 corners
				quad1.position = sf::Vector2f(LU.x, LU.y);
				quad2.position = sf::Vector2f(RU.x, RU.y);
				quad3.position = sf::Vector2f(RB.x, RB.y);
				quad4.position = sf::Vector2f(LB.x, LB.y);


				//sf::Rect<int> textcoor = font.getGlyph(glyph, 128, false).textureRect;

				// define its 4 texture coordinates
				//quad1.texCoords = sf::Vector2f(textcoor.left ,textcoor.top );
				//quad2.texCoords = sf::Vector2f(textcoor.left + textcoor .width, textcoor.top );
				//quad3.texCoords = sf::Vector2f(textcoor.left + textcoor.width, textcoor.top+ textcoor.height);
				//quad4.texCoords = sf::Vector2f(textcoor.left , textcoor.top +textcoor.height);
				char lettre = (glyphs.cells[cellCount]->ent != nullptr) ? glyphs.cells[cellCount]->ent->glyph : glyphs.cells[cellCount]->glyph;
				quad1.texCoords = sf::Vector2f((lettre - 31) * 128, 9);
				quad2.texCoords = sf::Vector2f(127 + (lettre - 31) * 128, 9);
				quad3.texCoords = sf::Vector2f(127 + (lettre - 31) * 128, 138);
				quad4.texCoords = sf::Vector2f((lettre - 31) * 128, 138);

				sf::Color color = (glyphs.cells[cellCount]->ent != nullptr) ? glyphs.cells[cellCount]->ent->baseColor : glyphs.cells[cellCount]->baseColor ;
				//Thanks a lot KUKURU3
				color.r = color.r * glyphs.lightCorrection[cellCount];
				color.g = color.g * glyphs.lightCorrection[cellCount];
				color.b = color.b * glyphs.lightCorrection[cellCount];
				quad1.color = color;
				quad2.color = color;
				quad3.color = color;
				quad4.color = color;

				Glyph glyph;

				glyph.vertices[0] = quad1;
				glyph.vertices[1] = quad2;
				glyph.vertices[2] = quad3;
				glyph.vertices[3] = quad4;
				glyph.orig = orig;
				glyph.cell = glyphs.cells[cellCount];
				returnedGlyph.push_back(glyph);
			
			}
			++cellCount;
		}
		return returnedGlyph;
	}
};

struct UI
{
	int mouseX;
	int mouseY;
	int mouseDown;

	int hotItem;
	int activeItem;

	//TODO : add pool of event

	void prepare()
	{
		hotItem = 0;
	}

	void finish()
	{
		if (mouseDown == 0)
		{
			activeItem = 0;
		}
		else
		{
			if (activeItem == 0)
				activeItem = -1; 
		}
	}
	void drawRect(int x, int y, int w, int h, const sf::Color& color,sf::Text& text, sf::RenderTexture& texture)
	{
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(400, 100));
		rectangle.setFillColor(color);
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(2);
		rectangle.setPosition(x, y);

		texture.draw(rectangle);
		texture.draw(text);
	}

	int regionHit(int x, int y, int w, int h)
	{
		if (mouseX < x || mouseY < y || mouseX >= x + w || mouseY >= y + h)
		{
			return 0;
		}
		return 1;
	}

	int button(int id, int x, int y, int w, int h )
	{
		if (regionHit(x, y, w, h))
		{
			hotItem = id;
			if (activeItem == 0 && mouseDown)
			{
				activeItem = id;
			}
		}

		//DRAW

		if (hotItem == id)
		{
			if (activeItem == id)
			{
			}
		}  
		
		// If button is hot and active, but mouse button is not
		// down, the user must have clicked the button.
		if (mouseDown == 0 &&
			hotItem == id &&
			activeItem == id)
			return 1;

		// Otherwise, no clicky.
		return 0;
	}

};





//
/* NOT USED, but in case of
	sf::Shader shader;


	//#version 150
	//	in vec4 gl3D_Position;
	//in vec4 gl_Color;

	//// GLSL Hacker automatic uniforms:
	//uniform mat4 gl3d_ModelViewMatrix;

	//out Vertex
	//{
	//  vec4 color;
	//} vertex;

	//void main()
	//{
	//	gl_Position = gxl3d_ModelViewMatrix * gxl3d_Position;
	//	vertex.color = gxl3d_Color;
	//}

	const std::string vertexShader = \
		//
		//"uniform mat4 gl_ModelViewMatrix; "\

		"out Vertex "\
		"{"\
		"  vec4 color; "\
		"} vertex;"\

		"void main() "\
		"{"\
		"	gl_Position = gl_ModelViewMatrix * gl_Position; "\
		"	vertex.color = gl_Color; "\
		"} ";

	//#version 150

	//	layout(sprites) in;
	//layout(triangle_strip) out;
	//layout(max_vertices = 4) out;

	//// GLSL Hacker automatic uniforms:
	//uniform mat4 gxl3d_ProjectionMatrix;

	//uniform float particle_size;

	//in Vertex
	//{
	//  vec4 color;
	//} vertex[];


	//out vec2 Vertex_UV;
	//out vec4 Vertex_Color;

	//void main(void)
	//{
	//	vec4 P = gl_in[0].gl_Position;

	//	// a: left-bottom
	//	vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
	//	gl_Position = gxl3d_ProjectionMatrix * vec4(va, P.zw);
	//	Vertex_UV = vec2(0.0, 0.0);
	//	Vertex_Color = vertex[0].color;
	//	EmitVertex();

	//	// b: left-top
	//	vec2 vb = P.xy + vec2(-0.5, 0.5) * particle_size;
	//	gl_Position = gxl3d_ProjectionMatrix * vec4(vb, P.zw);
	//	Vertex_UV = vec2(0.0, 1.0);
	//	Vertex_Color = vertex[0].color;
	//	EmitVertex();

	//	// d: right-bottom
	//	vec2 vd = P.xy + vec2(0.5, -0.5) * particle_size;
	//	gl_Position = gxl3d_ProjectionMatrix * vec4(vd, P.zw);
	//	Vertex_UV = vec2(1.0, 0.0);
	//	Vertex_Color = vertex[0].color;
	//	EmitVertex();

	//	// c: right-top
	//	vec2 vc = P.xy + vec2(0.5, 0.5) * particle_size;
	//	gl_Position = gxl3d_ProjectionMatrix * vec4(vc, P.zw);
	//	Vertex_UV = vec2(1.0, 1.0);
	//	Vertex_Color = vertex[0].color;
	//	EmitVertex();

	//	EndPrimitive();
	//}
	const std::string geometryShader = \
		"#version 330 core \n"\
		"layout (sprites) in; "\
		"layout (triangle_strip,max_vertices = 4) out; "\
		"void main() " \
		"{ " \
		"    vec4 P = gl_in[0].gl_Position; "\
		"} ";

	//#version 150
	//	uniform sampler2D tex0;
	//in vec2 Vertex_UV;
	//in vec4 Vertex_Color;
	//out vec4 FragColor;
	//void main(void)
	//{
	//	vec2 uv = Vertex_UV.xy;
	//	uv.y *= -1.0;
	//	vec3 t = texture(tex0, uv).rgb;
	//	FragColor = vec4(t, 1.0) * Vertex_Color;
	//}

	const std::string fragmentShader = \
		"uniform sampler2D texture;"\
		"void main()" \
		"{" \

		"	 vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);" \
		"	 gl_FragColor = gl_Color * pixel;" \
		"}";

	sf::VertexArray m_vertices;

*/