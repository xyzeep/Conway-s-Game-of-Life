#include <stdio.h>
#include <SDL3/SDL.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0xFFB0B0B0

#define CELL_SIZE 20
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLS SCREEN_WIDTH / CELL_SIZE
#define GRIDLINE_WIDTH 4



void draw_grid(SDL_Surface *surface) {
	

	// rows
	for (int i = 0; i < SCREEN_HEIGHT; i += CELL_SIZE) {
		SDL_Rect line = {0, i, SCREEN_WIDTH, GRIDLINE_WIDTH};
		SDL_FillSurfaceRect(surface, &line, COLOR_GRAY);
	}

	// for cols
	for (int i = 0; i < SCREEN_WIDTH; i += CELL_SIZE) {
		SDL_Rect line = {i, 0, GRIDLINE_WIDTH, SCREEN_WIDTH};
		SDL_FillSurfaceRect(surface, &line, COLOR_GRAY);

	}
	
}



int main() {
	// initializing SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Conway's Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Surface *surface = SDL_GetWindowSurface(window);

	//background
	SDL_Rect white_screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	int running = 1;

	SDL_Event event;
	
	while(running) {
		 while (SDL_PollEvent(&event)) {
           		 if (event.type == SDL_EVENT_QUIT) {
               			 running = 0;
			 }
		 }
		
		// re-draw white bg on each update
		SDL_FillSurfaceRect(surface, &white_screen, COLOR_WHITE);
		
		// draw grid
		draw_grid(surface);
	
	

		SDL_UpdateWindowSurface(window);
		

		SDL_Delay(200);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	
	return 0;
}
