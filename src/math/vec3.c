#include <math.h>
#include "vec3.h"

t_vec3	vec3_add(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x + b.x, a.y + b.y, a.z + b.z});
}

t_vec3	vec3_sub(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x - b.x, a.y - b.y, a.z - b.z});
}

t_vec3	vec3_scale(t_vec3 v, double t)
{
	return ((t_vec3){v.x * t, v.y * t, v.z * t});
}

double	vec3_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vec3	vec3_cross(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	});
}

double	vec3_length(t_vec3 v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

t_vec3	vec3_normalize(t_vec3 v)
{
	double	len;

	len = vec3_length(v);
	return (vec3_scale(v, 1.0 / len));
}

/* Linear interpolation: (1-t)*a + t*b */
t_vec3	vec3_lerp(t_vec3 a, t_vec3 b, double t)
{
	return (vec3_add(vec3_scale(a, 1.0 - t), vec3_scale(b, t)));
}
