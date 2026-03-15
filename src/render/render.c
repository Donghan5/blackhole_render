#include "render.h"
#include "../background/background.h"

/* Generate a ray for (x, y), shade it, and write the pixel */
static void	render_pixel(SDL_Surface *surface, t_camera camera, int x, int y)
{
	t_ray	ray;
	t_color	color;

	ray   = camera_get_ray(camera, x, y);
	color = background_color(ray);
	((Uint32 *)surface->pixels)[y * surface->w + x] =
		color_to_sdl(color, surface->format);
}

void	render(SDL_Surface *surface, t_camera camera)
{
	int	x;
	int	y;

	y = 0;
	while (y < camera.height)
	{
		x = 0;
		while (x < camera.width)
		{
			render_pixel(surface, camera, x, y);
			x++;
		}
		y++;
	}
}
