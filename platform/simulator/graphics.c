#include <SDL2/SDL.h>

#define WINDOW_TITLE             "OS Simulator"
#define WINDOW_WIDTH          320
#define WINDOW_HEIGHT         480

static u32 _pixels[WINDOW_HEIGHT * WINDOW_WIDTH];
static SDL_Texture *_framebuffer;
static SDL_Window *_window;
static SDL_Renderer *_renderer;

/* --- PUBLIC --- */
void graphics_init(void)
{
	/* Init SDL */
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error initializing SDL; SDL_Init: %s\n",
			SDL_GetError());
		goto ERROR_INIT_SDL;
	}

	/* Create SDL_Window */
	if(!(_window = SDL_CreateWindow(WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0)))
	{
		printf("Error creating SDL_Window: %s\n",
			SDL_GetError());
		goto ERROR_CREATE_WINDOW;
	}

	/* Create SDL_Renderer */
	if(!(_renderer = SDL_CreateRenderer
		(_window, -1, SDL_RENDERER_ACCELERATED)))
	{
		printf("Error creating SDL_Renderer: %s\n",
			SDL_GetError());
		goto ERROR_CREATE_RENDERER;
	}

	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

	_framebuffer = SDL_CreateTexture(_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH, WINDOW_HEIGHT);

	return;

ERROR_CREATE_RENDERER:
	SDL_DestroyWindow(_window);

ERROR_CREATE_WINDOW:
	SDL_Quit();

ERROR_INIT_SDL:
	exit(1);
}

void graphics_destroy(void)
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

u32 graphics_color(u8 r, u8 g, u8 b)
{
	return ((u32)r << 16) | ((u32)g << 8) | (u32)b;
}

void graphics_rect(i32 x, i32 y, i32 w, i32 h, u32 color)
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

void graphics_update(void)
{
	static SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
		case SDL_QUIT:
			exit(0);
			break;
		}
	}

	SDL_UpdateTexture(_framebuffer, NULL, _pixels, WINDOW_WIDTH * sizeof(u32));
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _framebuffer, NULL, NULL);
	SDL_RenderPresent(_renderer);
}

/*
	case SDL_KEYDOWN:
		if(event.key.keysym.sym == SDLK_ESCAPE)
		{
			running = 0;
			break;
		}

		os_event_key(event.key.keysym.sym, 0);
		break;

	case SDL_KEYUP:
		os_event_key(event.key.keysym.sym, 1);
		break;
*/
