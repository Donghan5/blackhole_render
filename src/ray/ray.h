#ifndef RAY_H
# define RAY_H

# include "../math/vec3.h"

typedef struct s_ray
{
	t_vec3	origin;
	t_vec3	direction;	/* must be normalised */
}	t_ray;

t_vec3	ray_at(t_ray ray, double t);

#endif
