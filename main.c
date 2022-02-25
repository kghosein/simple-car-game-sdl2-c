//Using SDL and standard IO
#include <SDL2/SDL.h>  
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// global variables
const int ScreenWidth = 800;
const int ScreenHeight = 600;
bool isRunning; // check if the game is running
SDL_Window *window; // window
SDL_Renderer *renderer; // renderer
SDL_Texture* playertex; // player sprite 
SDL_Texture* enemytex; // enemy sprite
SDL_Rect p_srcR, p_destR; // player source and destination rectangles
SDL_Rect e_srcR, e_destR; // enemy source and destination rectangles
static SDL_Event event; // event
int counter;

// declare functions
// initialize SDL
void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

// handle events
void handleEvents();

// collision detection
static bool AABB(SDL_Rect recA, SDL_Rect recB);

// go through all game objects and update them (this is where we would write mainly write the game logic)
void update();

// texture manager
static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren);

//Render objects to screen
void render(); 

// release resources
void clean();

// to check if the game is running
bool running() { return isRunning; }

int main( int argc, char* args[] )
{
	const int FPS = 60;
	const int frameDelay = 500 / FPS;
	uint32_t frameStart;
	int frameTime;

	init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, false);

	bool flag = true; // winning condition
	
	// game loop
	while( running() )
	{
		frameStart = SDL_GetTicks();
		
		// handle user events
		handleEvents();

		// update the game
		update();

		// render to the screen
		render();

		frameTime = SDL_GetTicks() - frameStart;

		if( frameDelay > frameTime )
		{
			SDL_Delay( frameDelay - frameTime );
		}

		if(p_destR.x >= ScreenWidth / 1.88 && flag) 
		{ 
			printf("\n\t*No Collision Detected!*\n\t*You Won!*\n\n"); 
			flag = false;
		}
	}

	clean();

	return 0;
}

// define functions

void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen){
	int flags = 0;
	if(fullscreen){
		flags = SDL_WINDOW_FULLSCREEN;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		printf("SDL initialized!\n");
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if(window) {
			printf("Window created!\n");
			}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if(renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			printf("Renderer created!\n");
			}
		isRunning = true;
	}
	else {
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		isRunning = false;
	}

	playertex = LoadTexture("assets/player.bmp", renderer);
	enemytex = LoadTexture("assets/enemy.bmp", renderer);
}

void handleEvents(){
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
	}
}

void update(){
	// car position
	p_destR.w = 100;
	p_destR.h = 100;
	p_destR.y = ScreenHeight / 2;

	// on key press
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		switch(event.key.keysym.sym) {
			case SDLK_RIGHT:
				p_destR.x += 5;
				break;
			case SDLK_UP:
				p_destR.y -= 150;
				p_destR.x += 3;
				break;
			default:
				break;
		}
	}

	// on key release
	else if(event.type == SDL_KEYUP && event.key.repeat == 0) {
		switch(event.key.keysym.sym) {
			case SDLK_UP:
				p_destR.y = ScreenHeight / 2;
				break;
			default:
				break;
		}
	}
	
	else {}
	
	// enemy position
	e_destR.w = 45;
	e_destR.h = 45;
	e_destR.x = ScreenWidth / 2 - e_destR.w / 2;
	e_destR.y = ScreenHeight / 1.6 - e_destR.h / 1.6;

	// collision detection and game over
	if(AABB(p_destR, e_destR)) {
		printf("\n\t*Collision Detected!*\n");
		printf("\t*Game Over!*\n\n");
		isRunning = false;
	}
	
}

void render(){
	// this is where we add stuff (textutes) to render
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, playertex, NULL, &p_destR);
	SDL_RenderCopy(renderer, enemytex, NULL, &e_destR);
	SDL_RenderPresent(renderer);
}

void clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	printf("Game cleaned!\n");
}

static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(fileName);
	if(loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", fileName, IMG_GetError());
	}
	else {
		newTexture = SDL_CreateTextureFromSurface(ren, loadedSurface);
		if(newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", fileName, SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

static bool AABB(SDL_Rect recA, SDL_Rect recB) {
	if(
		recA.x + recA.w >= recB.x && 
		recB.x + recB.w >= recA.x && 
		recA.y + recA.h >= recB.y && 
		recB.y + recB.h >= recA.y) {
		return true;
	}
	return false;
}