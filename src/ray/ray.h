#ifndef RAY_H
# define RAY_H

# include "../math/vec3.h"

typedef struct s_ray
{
	t_vec3	origin;
	t_vec3	direction;	/* must be normalised before use */
}	t_ray;

/* Returns the world-space point along the ray at parameter t: origin + t*dir */
t_vec3	ray_at(t_ray ray, double t);

#endif
