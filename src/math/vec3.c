#include <math.h>
#include "vec3.h"

/* Returns a + b component-wise. */
t_vec3	vec3_add(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x + b.x, a.y + b.y, a.z + b.z});
}

/* Returns a - b component-wise. */
t_vec3	vec3_sub(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x - b.x, a.y - b.y, a.z - b.z});
}

/* Returns a scaled by scalar t. */
t_vec3	vec3_scale(t_vec3 a, double t)
{
	return ((t_vec3){a.x * t, a.y * t, a.z * t});
}

/* Returns the dot product a · b. */
double	vec3_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

/* Returns |a|² (squared length — avoids sqrt when only comparing distances). */
double	vec3_length_sq(t_vec3 a)
{
	return (vec3_dot(a, a));
}

/* Returns |a| (Euclidean length). */
double	vec3_length(t_vec3 a)
{
	return (sqrt(vec3_length_sq(a)));
}

/* Returns a unit vector in the direction of a.
   Caller must ensure a is not the zero vector. */
t_vec3	vec3_normalize(t_vec3 a)
{
	return (vec3_scale(a, 1.0 / vec3_length(a)));
}

/* Returns a × b (cross product — used to build orthonormal camera bases). */
t_vec3	vec3_cross(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	});
}
