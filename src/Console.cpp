#include "Console.h"


struct Console
{


	typedef struct 
	{
		asciiChar glyph;
		uint32 fgColor; 
		uint32 bgColor;
	} Cell;

	typedef struct
	{
		
		uint32 atlasWidth;
		uint32 atlasHeight;
		uint32 charWidth;
		uint32 charHeight;
		asciiChar firstCharInAtlas;
		uint32* atlas;
	}Font;
	 
	typedef struct
	{
		int32 x;
		int32 y;
		int32 w;
		int32 h;
	} Rect;

	uint32* pixels;
	uint32 width;
	uint32 height;
	uint32 colCount;
	uint32 rowCount;
	uint32 cellWidth;
	uint32 cellHeight;
	Font* font;
	Cell* cells;

	void PT_ConsoleClear();
	void ConsoleSetBitmapFont();

	//void ColorizePixel(); //use the source color to calculate thedestination pixels
	//CopyBlend() // ref: https://en.wikipedia.org/wiki/Alpha_compositing
	//		// Colorize our source pixel before we blend it
	//		
	//FillBlend()
	//	// For each pixel in the destination rect, alpha blend the 
	//	// bgColor to the existing color.

	//Fill() 
	//	// only ysed for clear (no blend)
	//void PT_ConsolePutCharAt();
	//		// Fill the background with alpha blending
	//		// Copy the glyph with alpha blending and desired coloring

	//Rect rectFromGlyph(asciiChar c)
	//	//calculate glyph position from char num

	Console(uint32 width, uint32 height, uint32 rowCount, uint32 colCount)
	{

		//this->pixels = (uint32*)calloc(width * height, sizeof(uint32));
		this->pixels = new uint32[width * height];
		this->width = width;
		this->height = height;
		this->rowCount = rowCount;
		this->colCount = colCount;
		this->cellWidth = width / colCount;
		this->cellHeight = height / rowCount;

		this->font = NULL;

		this->cells = new Cell[colCount * rowCount];


	}
	~Console()
	{

		//free(pixels);
		delete[] pixels;
		delete font;
		delete cells;
	}
	//Console(const Console&);
	//Console& operator=(const Console&);


	void initConsoleFont(std::string& filename )
	{
		int imgWidth, imgHeight, numComponents;

		unsigned char* imgData = stbi_load(filename.c_str(),
			&imgWidth, &imgHeight,
			&numComponents, STBI_rgb_alpha);

		uint32 imageDatasize = imgWidth * imgHeight * sizeof(uint32);

		

		stbi_image_free(imgData);

		if (this->font != NULL)
		{
			
		}

	}

};