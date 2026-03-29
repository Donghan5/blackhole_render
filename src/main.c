#include <SDL2/SDL.h>
#include <stdio.h>
#include "camera/camera.h"
#include "render/render.h"
#include "background/background.h"

#define WIDTH	800
#define HEIGHT	600

static int	init_sdl(SDL_Window **window, SDL_Surface **surface)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Error: SDL_Init failed: %s\n", SDL_GetError());
		return (0);
	}
	*window = SDL_CreateWindow("Black Hole Renderer",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, 0);
	if (!*window)
	{
		fprintf(stderr, "Error: SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return (0);
	}
	*surface = SDL_GetWindowSurface(*window);
	return (1);
}

static void	event_loop(SDL_Window *window)
{
	SDL_Event	e;

	SDL_UpdateWindowSurface(window);
	while (SDL_WaitEvent(&e))
		if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
			break ;
}

int	main(void)
{
	SDL_Window	*window;
	SDL_Surface	*surface;
	t_camera	camera;
	t_vec3		pos;
	t_vec3		target;
	t_vec3		up;

	if (!init_sdl(&window, &surface))
		return (1);
	/*
	** Camera placed at ~17° above the equatorial plane, 20 units out.
	** This angle reveals both the disk's top face and the lensed image
	** of the bottom face bent over the black hole.
	*/
	pos    = (t_vec3){0.0, 6.0, 18.0};
	target = (t_vec3){0.0, 0.0,  0.0};
	up     = (t_vec3){0.0, 1.0,  0.0};
	camera = camera_init(pos, target, up, 55.0, WIDTH, HEIGHT);
	if (!skybox_init("assets/skybox.avif"))
		fprintf(stderr, "Warning: skybox not loaded — using black background\n");
	render(surface, camera);
	event_loop(window);
	skybox_destroy();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return (0);
}
