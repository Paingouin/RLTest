#include "main.h"

struct Camera
{
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 LastPosition;

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float fov = 90.0f;
	//double S = 1.0 / (glm::tan(fov / 2.0));
	float near = 0.1f;
	float far = 100.f;
	float heightOfNearPlane;

	glm::vec4 viewport;
	glm::vec4 camRight;
	glm::vec4 camUp;
	glm::vec3 camFront;
	glm::mat4 model;
	glm::mat4 mRotate;
	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::mat4 mModelView;
	glm::mat4 mTotal;

	sf::VertexArray m_vertices;



	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors(glm::vec3 target)
	{
		// Calculate the new position of the camera
		glm::vec3 posOffset;
		posOffset.x = Zoom * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		posOffset.y = Zoom * sin(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		posOffset.z = Zoom * cos(glm::radians(Yaw));

		Position += posOffset;

		glm::mat4 mScale =
		{
			 1.f , 0.f, 0.f, 0.f,
			 0.f , 1.f, 0.f, 0.f,
			 0.f , 0.f, 1.f, 0.f,
			 0.f , 0.f, 0.f, 1.f
		};
		glm::mat4 mTranslate =
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
		 //model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first;
	 //translation*rotation*scale
		glm::vec4 camPos = {(float) Position.x , (float)Position.y ,(float)Position.z  , 1.f };
		glm::vec4 tarPos = { (float) target.x  , (float)target.y , (float)target.z  , 1.f };

		mRotate = glm::rotate(glm::radians(-Pitch - 180), camFront);
		model = mTranslate * mScale;
		glm::vec3 camPos3 = model * camPos;
		glm::vec3 tarPos3 = model * tarPos;
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
		mTotal = mProjection * mView  * model;
		mModelView = mView * model; //now we can use the camera View space (so in2d from the camera)

	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
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


	//Calculate Global position on screen based of camera
	const Glyph to_global(float x, float y, float z, char glyph, sf::Color& color, sf::Font& font)
	{
		Glyph result = {};

		glm::vec4 orig = { x, y, z, 1.f };
		
		glm::vec4 pos = mModelView * (orig* 1.5f);
		glm::vec4 LU = { pos.x - 0.5 , pos.y + 0.5, pos.z ,1.f };
		glm::vec4 RU = { pos.x + 0.5 , pos.y + 0.5, pos.z ,1.f };
		glm::vec4 RB = { pos.x + 0.5 , pos.y - 0.5, pos.z ,1.f };
		glm::vec4 LB = { pos.x - 0.5 , pos.y - 0.5, pos.z ,1.f };



		LU = mProjection * LU;
		RU = mProjection * RU;
		RB = mProjection * RB;
		LB = mProjection * LB;
		
		//if ((LU.z > -1.f && LU.z < 1.f)&&(RU.z > -1.f && RU.z < 1.f)&& (RB.z > -1.f && RB.z < 1.f) && (LB.z > -1.f && LB.z < 1.f)) //culling
		//{
			LU /= LU.w;
			RU /= RU.w;
			RB /= RB.w;
			LB /= LB.w;


			LU.x = (LU.x * viewport[2]) + viewport[2]/2;
			LU.y = (viewport[3]/2 - (LU.y * viewport[3]));
			RU.x = (RU.x * viewport[2]) + viewport[2] / 2;
			RU.y = (viewport[3] / 2 - (RU.y * viewport[3])) ;
			RB.x = (RB.x * viewport[2]) + viewport[2] / 2;
			RB.y = (viewport[3] / 2 - (RB.y * viewport[3]));
			LB.x = (LB.x * viewport[2]) + viewport[2] / 2;
			LB.y = (viewport[3] / 2 - (LB.y * viewport[3]));

			orig = mTotal * orig;
			orig.z /= orig.w;
			//float distance = mFinal.w; // /!\ the distance is given by the w value, not the Z
			//orig.x /= orig.w;
			//orig.y /= orig.w;
			
			//orig /= orig.w;
			//scale = heightOfNearPlane * 0.015f / distance;
			//orig = orig * 0.5f + 0.5f;

			sf::Vertex quad1 = {};
			sf::Vertex quad2 = {};
			sf::Vertex quad3 = {};
			sf::Vertex quad4 = {};
			// define its 4 corners
			quad1.position = sf::Vector2f(LU.x, LU.y);
			quad2.position = sf::Vector2f(RU.x, RU.y);
			quad3.position = sf::Vector2f(RB.x, RB.y);
			quad4.position = sf::Vector2f(LB.x, LB.y);

			//TODO(Generate these at start in hashmap)
			sf::Rect<int> textcoor = font.getGlyph(glyph, 128, false).textureRect;

			// define its 4 texture coordinates
			quad1.texCoords = sf::Vector2f(textcoor.left ,textcoor.top );
			quad2.texCoords = sf::Vector2f(textcoor.left + textcoor .width, textcoor.top );
			quad3.texCoords = sf::Vector2f(textcoor.left + textcoor.width, textcoor.top+ textcoor.height);
			quad4.texCoords = sf::Vector2f(textcoor.left , textcoor.top +textcoor.height);

			quad1.color = color;
			quad2.color = color;
			quad3.color = color;
			quad4.color = color;

			result.vertices[0] = quad1;
			result.vertices[1] = quad2;
			result.vertices[2] = quad3;
			result.vertices[3] = quad4;
			result.orig = orig;
			return result;
		//}
		//else
		//{
		//	result.orig = orig;
		//	result.orig.z = -1;
		//	return result;
		//}

		//orig = glm::rotate(Pitch, glm::vec3(0.f, 0.f, 1.f)) * orig;


	//	glm::vec3 LU = orig - (camRight - camUp) * 0.6f * 0.5f;
	//	glm::vec3 RU = orig + (camRight + camUp) * 0.6f * 0.5f;
	//	glm::vec3 RB = orig + (camRight - camUp) * 0.6f * 0.5f;
	//	glm::vec3 LB = orig - (camRight + camUp) * 0.6f * 0.5f;
	//    
	//	/*LU -= orig;
	//	RU -= orig;
	//	RB -= orig;
	//	LB -= orig;

	//	LU = mRotate * LU;
	//	RU = mRotate * RU;
	//	RB = mRotate * RB;
	//	LB = mRotate * LB;

	//	LU += orig;
	//	RU += orig;
	//	RB += orig;
	//	LB += orig;*/

	//	
	//	LU = mTotal * LU;
	//	RU = mTotal * RU;
	//	RB = mTotal * RB;
	//	LB = mTotal * LB;

	//	LU /= LU.w;
	//	RU /= RU.w;
	//	RB /= RB.w;
	//	LB /= LB.w;

	//	LU = LU * 0.5f + 0.5f;
	//	RU = RU * 0.5f + 0.5f;
	//	RB = RB * 0.5f + 0.5f;
	//	LB = LB * 0.5f + 0.5f;

	//	
	//	if ((LU.z > -1.f && LU.z < 1.f)&&(RU.z > -1.f && RU.z < 1.f)&& (RB.z > -1.f && RB.z < 1.f) && (LB.z > -1.f && LB.z < 1.f)) //culling
	//	{
	//		LU.x = (LU.x * viewport[2]) + viewport[0];
	//		LU.y = (viewport[3] - (LU.y * viewport[3])) + viewport[1];
	//		RU.x = (RU.x * viewport[2]) + viewport[0];
	//		RU.y = (viewport[3] - (RU.y * viewport[3])) + viewport[1];
	//		RB.x = (RB.x * viewport[2]) + viewport[0];
	//		RB.y = (viewport[3] - (RB.y * viewport[3])) + viewport[1];
	//		LB.x = (LB.x * viewport[2]) + viewport[0];
	//		LB.y = (viewport[3] - (LB.y * viewport[3])) + viewport[1];

	//		orig = mTotal * orig;

	//		//float distance = mFinal.w; // /!\ the distance is given by the w value, not the Z
	//		orig.x /= orig.w;
	//		orig.y /= orig.w;
	//		orig.z /= orig.w;
	//		//orig /= orig.w;
	//		//scale = heightOfNearPlane * 0.015f / distance;
	//		orig = orig * 0.5f + 0.5f;

	//		sf::Vertex quad1 = {};
	//		sf::Vertex quad2 = {};
	//		sf::Vertex quad3 = {};
	//		sf::Vertex quad4 = {};
	//		// define its 4 corners
	//		quad1.position = sf::Vector2f(LU.x, LU.y);
	//		quad2.position = sf::Vector2f(RU.x, RU.y);
	//		quad3.position = sf::Vector2f(RB.x, RB.y);
	//		quad4.position = sf::Vector2f(LB.x, LB.y);

	//		sf::Rect<int> textcoor = font.getGlyph(glyph, 128, false).textureRect;

	//		// define its 4 texture coordinates
	//		quad1.texCoords = sf::Vector2f(textcoor.left ,textcoor.top );
	//		quad2.texCoords = sf::Vector2f(textcoor.left + textcoor .width, textcoor.top );
	//		quad3.texCoords = sf::Vector2f(textcoor.left + textcoor.width, textcoor.top+ textcoor.height);
	//		quad4.texCoords = sf::Vector2f(textcoor.left , textcoor.top +textcoor.height);

	//		quad1.color = color;
	//		quad2.color = color;
	//		quad3.color = color;
	//		quad4.color = color;

	//		result.vertices[0] = quad1;
	//		result.vertices[1] = quad2;
	//		result.vertices[2] = quad3;
	//		result.vertices[3] = quad4;
	//		result.orig = orig;
	//		return result;
	//	}
	//	else
	//	{
	//		result.orig = orig;
	//		result.orig.z = -1;
	//		return result;
	//	}

	//	//mFinal = mFinal * 0.5f + 0.5f;
	//	//To do Z0(0,1) (NO par default) :  comment above and uncomment below
	//	//mFinal.x = mFinal.x * 0.5 + 0.5;
	//	//mFinal.y = mFinal.y * 0.5 + 0.5;

	//	//winX = (mFinal.x * viewport[2]) + viewport[0];
	//	//winY = (viewport[3] - (mFinal.y * viewport[3])) + viewport[1];
	}

};

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

	//	layout(points) in;
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
		"layout (points) in; "\
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