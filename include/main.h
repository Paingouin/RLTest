#pragma once 

#include <stdbool.h>
#include <stdint.h>
#include <string>

typedef unsigned char asciiChar;

typedef uint8_t		uint8;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef int32_t		int32;
typedef int64_t		int64;

#define internal static
#define local_persist static
#define global_variable static

// Include the STB image library - only the PNG support
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"
/*
 unsigned char *imgData = stbi_load(filename,
									&imgWidth, &imgHeight,
									&numComponents, STBI_rgb_alpha);


	do a malloc, get all data on memory and free

stbi_image_free(imgData);

*/

// Helper macros for working with pixel colors
#define RED(c) ((c & 0xff000000) >> 24)
#define GREEN(c) ((c & 0x00ff0000) >> 16)
#define BLUE(c) ((c & 0x0000ff00) >> 8)
#define ALPHA(c) (c & 0xff)

#define COLOR_FROM_RGBA(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)



