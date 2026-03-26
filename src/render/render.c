#include <string.h>
#include "render.h"
#include "../background/background.h"
#include "../geodesic/geodesic.h"
#include "../accretion/accretion.h"

/* RK4 step size and iteration cap for ray tracing */
#define TRACE_DT         0.1
#define TRACE_MAX_STEPS  10000

/* Writes a packed pixel directly into the surface's pixel buffer. */
static void	put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	Uint32	*pixels;

	pixels = (Uint32 *)surface->pixels;
	pixels[y * surface->w + x] = pixel;
}

/* Build a 6-element geodesic state vector from a camera ray. */
static void	init_state(t_ray ray, double *state)
{
	state[0] = ray.origin.x;
	state[1] = ray.origin.y;
	state[2] = ray.origin.z;
	state[3] = ray.direction.x;
	state[4] = ray.direction.y;
	state[5] = ray.direction.z;
}

/* Build a t_ray from the escaped photon's final velocity direction. */
static t_ray	make_escape_ray(double *state)
{
	t_ray	ray;
	t_vec3	dir;

	dir = (t_vec3){state[3], state[4], state[5]};
	ray.origin    = (t_vec3){0.0, 0.0, 0.0};
	ray.direction = vec3_normalize(dir);
	return (ray);
}

/*
** After one RK4 step, check all termination conditions in priority order:
**  1. Disk crossing  → disk color
**  2. Absorbed (r < rs/2)  → black
**  3. Escaped (r > r_max)  → background
** Returns 1 and fills *out if the ray has terminated; 0 to keep integrating.
*/
static int	step_result(double *prev, double *state, t_color *out)
{
	t_disk_hit	disk;
	double		r2;

	disk = accretion_check_crossing(prev, state);
	if (disk.hit)
	{
		*out = accretion_color(disk);
		return (1);
	}
	r2 = state[0] * state[0] + state[1] * state[1] + state[2] * state[2];
	if (r2 < GEODESIC_RS_HALF * GEODESIC_RS_HALF)
	{
		*out = (t_color){0.0, 0.0, 0.0};
		return (1);
	}
	if (r2 > GEODESIC_R_MAX * GEODESIC_R_MAX)
	{
		*out = background_color(make_escape_ray(state));
		return (1);
	}
	return (0);
}

/*
** trace_ray: follow a photon through curved Schwarzschild spacetime.
**
** Initialises a 6D geodesic state from the camera ray, then advances it
** step by step using rk4_step(geodesic_deriv, ...).  After each step the
** accretion disk and termination conditions are evaluated.
*/
static t_color	trace_ray(t_ray ray)
{
	double	state[6];
	double	prev[6];
	t_color	out;
	int		step;

	init_state(ray, state);
	step = 0;
	while (step < TRACE_MAX_STEPS)
	{
		memcpy(prev, state, 6 * sizeof(double));
		rk4_step(geodesic_deriv, step * TRACE_DT, state, 6, TRACE_DT, NULL);
		if (step_result(prev, state, &out))
			return (out);
		step++;
	}
	/* Timed-out ray treated as escaped */
	return (background_color(make_escape_ray(state)));
}

/* Renders one horizontal scanline at row y. */
static void	render_row(SDL_Surface *surface, t_camera camera, int y)
{
	int		x;
	t_ray	ray;
	t_color	color;

	x = 0;
	while (x < camera.width)
	{
		ray   = camera_get_ray(camera, x, y);
		color = trace_ray(ray);
		put_pixel(surface, x, y, color_to_sdl(color, surface->format));
		x++;
	}
}

/* Locks the surface, renders all pixels row by row, then unlocks. */
void	render(SDL_Surface *surface, t_camera camera)
{
	int	y;

	SDL_LockSurface(surface);
	y = 0;
	while (y < camera.height)
	{
		render_row(surface, camera, y);
		y++;
	}
	SDL_UnlockSurface(surface);
}
