#ifndef RENDER_H
# define RENDER_H

# include <SDL2/SDL.h>
# include "../camera/camera.h"

/* Renders the full frame into surface by shooting one ray per pixel. */
void	render(SDL_Surface *surface, t_camera camera);

#endif
