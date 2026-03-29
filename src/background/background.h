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

/* Load an AVIF skybox image from path. Returns 1 on success, 0 on failure. */
int		skybox_init(const char *path);

/* Free the skybox pixel buffer. */
void	skybox_destroy(void);

/* ray.direction must be normalised before calling */
t_color	background_color(t_ray ray);
Uint32	color_to_sdl(t_color color, SDL_PixelFormat *format);

#endif
