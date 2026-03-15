#include "ray.h"

/* Returns the point along the ray at parameter t: P = origin + t * direction */
t_vec3	ray_at(t_ray ray, double t)
{
	return (vec3_add(ray.origin, vec3_scale(ray.direction, t)));
}
