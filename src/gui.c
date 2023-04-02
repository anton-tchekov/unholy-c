#include "gui.h"
#include <SDL2/SDL.h>

#define WINDOW_TITLE             "OS GUI Simulator"

static u32 _pixels[WINDOW_HEIGHT * WINDOW_WIDTH];

static int _ctor(SDL_Window **window, SDL_Renderer **renderer);
static void _dtor(SDL_Window *window, SDL_Renderer *renderer);

	SDL_Texture *framebuffer;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	int running = 1;
	SDL_Event event;

void os_init(void)
{
	if(_ctor(&window, &renderer))
	{
		return;
	}

	framebuffer = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH, WINDOW_HEIGHT);

}

Color render_color(u8 r, u8 g, u8 b)
{
	return ((u32)r << 16) | ((u32)g << 8) | ((u32)b << 0);
}

void render_rect(i32 x, i32 y, i32 w, i32 h, Color color)
{
	i32 x0, y0;
	for(y0 = y; y0 < y + h; ++y0)
	{
		for(x0 = x; x0 < x + w; ++x0)
		{
			_pixels[y0 * WINDOW_WIDTH + x0] = color;
		}
	}
}

static int _ctor(SDL_Window **window, SDL_Renderer **renderer)
{
	/* Init SDL */
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error initializing SDL; SDL_Init: %s\n",
			SDL_GetError());
		goto ERROR_INIT_SDL;
	}

	/* Create SDL_Window */
	if((*window = SDL_CreateWindow(WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0)) == NULL)
	{
		printf("Error creating SDL_Window: %s\n",
			SDL_GetError());
		goto ERROR_CREATE_WINDOW;
	}

	/* Create SDL_Renderer */
	if((*renderer = SDL_CreateRenderer
		(*window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
	{
		printf("Error creating SDL_Renderer: %s\n",
			SDL_GetError());
		goto ERROR_CREATE_RENDERER;
	}

	SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
	return 0;

ERROR_CREATE_RENDERER:
	SDL_DestroyWindow(*window);

ERROR_CREATE_WINDOW:
	SDL_Quit();

ERROR_INIT_SDL:
	return 1;
}

static void _dtor(SDL_Window *window, SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

