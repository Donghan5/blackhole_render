#ifndef BACKGROUND_H
# define BACKGROUND_H

# include <SDL2/SDL.h>
# include "../ray/ray.h"

typedef struct s_color
{
	double	r;
	double	g;
	double	b;
}	t_color;

/* ray.direction must be normalised before calling */
t_color	background_color(t_ray ray);
Uint32	color_to_sdl(t_color color, SDL_PixelFormat *format);

#endif
