#include <SDL.h>
#include "Console.h"

#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

#define NUM_COLS	80
#define NUM_ROWS	45

void render_screen(SDL_Renderer* renderer, SDL_Texture* screen, uint32* pixels)
{
	SDL_UpdateTexture(screen, NULL, pixels, SCREEN_WIDTH * sizeof(uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Test RL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	int x = 8, y = 8;

	SDL_Texture* sprite = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	
	bool done = false;
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {

			if (event.type == SDL_QUIT) {
				done = true;
				break;
			}

		}

		render_screen(renderer, screen, console.pixels);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}