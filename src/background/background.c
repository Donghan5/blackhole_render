#include "background.h"

/* Deep-space black background. ray parameter kept for future skybox support. */
t_color	background_color(t_ray ray)
{
	(void)ray;
	return ((t_color){0.0, 0.0, 0.0});
}

/* Convert floating-point color in [0,1] to a packed SDL Uint32 pixel */
Uint32	color_to_sdl(t_color color, SDL_PixelFormat *format)
{
	Uint8	r;
	Uint8	g;
	Uint8	b;

	r = (Uint8)(color.r * 255.999);
	g = (Uint8)(color.g * 255.999);
	b = (Uint8)(color.b * 255.999);
	return (SDL_MapRGB(format, r, g, b));
}
