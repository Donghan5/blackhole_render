#include "ray.h"

/* Returns origin + t * direction — the point the ray reaches at parameter t. */
t_vec3	ray_at(t_ray ray, double t)
{
	return (vec3_add(ray.origin, vec3_scale(ray.direction, t)));
}
