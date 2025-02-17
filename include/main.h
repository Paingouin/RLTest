#pragma once 


#include <SFML/Graphics.hpp>

#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_INLINE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_ALIGNED_GENTYPES

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::dmat4
#include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_aligned.hpp"

#include "Mix/World.h"


#include "game.h"

#define POST_SHADER_FILENAME         "./Shaders/post.frag"
#define HEAT_SHADER_FILENAME         "./Shaders/heat.frag"
#define FONT_FILENAME			     "./Fonts/square.ttf"


// Include the STB image library - only the PNG support
//#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_PNG
//#include "stb_image.h"



/*
typedef unsigned char asciiChar;


*/

/*

 unsigned char *imgData = stbi_load(filename,
									&imgWidth, &imgHeight,
									&numComponents, STBI_rgb_alpha);

	do a malloc, get all data on memory and free

stbi_image_free(imgData);

*/


/*-------------------------------------------------------------------------------------------------------------------------*/


//GAME

struct PositionComponent
{
	PositionComponent(float x = 0, float y = 0 , float z = 0) : x(x), y(y), z(z) {}
	float x, y, z;
};


class MoveSystem : public Mix::System
{
public:
	MoveSystem()
	{
		// which components an entity must have for the system to be interested
		requireComponent<PositionComponent>();
		//requireComponent<VelocityComponent>();
	}

	void update()
	{
		// do stuff with all the entities of interest
		for (auto e : getEntities()) {
			auto& position = e.getComponent<PositionComponent>();
		//	const auto& velocity = e.getComponent<VelocityComponent>();
		//	position.x += velocity.x;
		//	position.y += velocity.y;
		}
	}
};

struct LightSource
{
	float radius;
	sf::Color color;
};

struct Entity
{
	char glyph;
	float x, y, z;

	sf::Color baseColor;
	LightSource* light = nullptr;
};

struct Cell
{
	bool block = false;
	bool visible = true;

	char glyph;
	float x, y, z;

	float lightLevel;
	float lightLevelToAdd;
	sf::Color baseColor;
	sf::Color colorToAdd;
	Entity* ent = nullptr;
	LightSource* light = nullptr;

};

struct Map
{
	unsigned int width, height;
	std::vector<Cell> cells;

	inline Cell& at(int x, int y)
	{
		return cells[x + (y * width)];
	}

	void moveEntity(Entity& ent, int deltaX, int deltaY)
	{
		int moveX = ent.x + deltaX;
		int moveY = ent.y + deltaY;
		if (moveX >= 0 && moveX < width && moveY >= 0 && moveY < height)
		{
			at(moveX, moveY).ent = &ent;
			at(ent.x, ent.y).ent = nullptr;
			ent.x = moveX;
			ent.y = moveY;
		}
	}
	
	void genRectangleRoom(int sizeX, int sizeY)
	{
		width = sizeX;
		height = sizeY;

		for (int y = 0; y < width; ++y)
		{
			for (int x = 0; x < height; ++x)
			{
				Cell cell;
				if ((x == 0 || x == width-1) || (y == 0 || y == height-1))
				{
					cell.glyph = '#';
					cell.z = 0;
					cell.block = true;
				}
				else
				{
					cell.glyph = '+';// y+48;
					cell.z = (x >= 3 && y <=3)? (x -3.f)*0.1 : 0.f;
				}
				cell.x = x;
				cell.y = y;
				cell.baseColor = sf::Color::Black;
				cells.push_back(cell);
			}
		}

		at(6, 6).block = true;
		at(6, 6).glyph = '#';
		at(7, 6).block = true;
		at(7, 6).glyph = '#';
		at(10, 6).block = true;
		at(10, 6).glyph = '#';

		at(11, 6).light = new LightSource;
		at(11, 6).light->radius = 8;
		at(11, 6).light->color = sf::Color(243, 204, 252);

		at(11, 19).light = new LightSource;
		at(11, 19).light->radius = 8;
		at(11, 19).light->color = sf::Color(204, 252, 250);

		at(10, 16).block = true;
		at(10, 16).glyph = '#';
		at(10, 18).block = true;
		at(10, 18).glyph = '#';
	}


void castLight(int row, float startX, float startY, float startSlope, float endSlope, int xx, int xy, int yx, int yy, int radius, sf::Color light= sf::Color::Black)
	{//NOTE : always start at row 1 at minimum
		float newStartSlope = 0.f;
		bool blocked = false;

		if (startSlope < endSlope) {
			return;
		}

		for(int distance = row ; distance <= radius && !blocked; ++distance)
		{
			int deltaY = -distance;
			for (int deltaX = -distance ; deltaX <= 0 ; ++deltaX)
			{
				int currentX = startX + deltaX * xx + deltaY * xy;
				int currentY = startY + deltaX * yx + deltaY * yy;

				float leftSlope = (deltaX - 0.5f) / (deltaY + 0.5f);
				float rightSlope = (deltaX + 0.5f) / (deltaY - 0.5f);

				if (!(currentX >= 0 && currentY >= 0 && currentX < width && currentY < height) || startSlope < rightSlope)
					continue;
				 else if (endSlope > leftSlope) {
					 break;
				}	

				glm::aligned_vec2 startPoint = { startX, startY };
				glm::aligned_vec2 endPoint = { currentX, currentY };
				float lineDistance = glm::distance(startPoint, endPoint);
				//check if it's within the lightable area and light if needed
				if (lineDistance <= radius) {
					if (light != sf::Color::Black)
					{
						//Multiple light attenuation!  https://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
						//clamp(1.0 - dist/radius, 0.0, 1.0); att *= att
						//1.0 / (radius*radius * minLight)
						//att = 1.0 / (1.0 + 0.1*dist + 0.01*dist*dist)

						float   bright = glm::clamp(1.0 - (lineDistance * lineDistance) / (radius * radius), 0.0, 1.0);
						bright *= bright;

						at(currentX, currentY).colorToAdd = light;
						at(currentX, currentY).lightLevel = bright;

						if (at(currentX, currentY).ent != nullptr)
						{
							at(currentX, currentY).ent->baseColor = light;
						}
					}
					else
					{
						at(currentX, currentY).visible = true;
					}	
				}

				if (blocked) { //previous cell was a blocking one
					if (at(currentX,currentY).block) {//hit a wall
						newStartSlope = rightSlope;
						continue;
					}
					else {
						blocked = false;
						startSlope = newStartSlope;
					}
				}
				else {
					if (at(currentX, currentY).block && distance < radius) {//hit a wall within sight line
						blocked = true;
						castLight(distance + 1, startX,startY, startSlope, leftSlope,xx, xy, yx, yy, radius, light);
						newStartSlope = rightSlope;
					}
				}
			}
		}
	}
};

struct Glyph
{
	sf::Vertex vertices[4]; //pos, LU ,RU, RB, LB
	Cell* cell;
	glm::aligned_vec4 orig;

	//Shader (link to shader 
};


struct Glyphs
{
	std::vector<glm::aligned_vec4> coordinates; //pos, LU ,RU, RB, LB
	std::vector<float> lightCorrection;
	std::vector<Cell*> cells;
};
