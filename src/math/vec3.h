#ifndef VEC3_H
#define VEC3_H

typedef struct s_vec3
{
	double	x;
	double	y;
	double	z;
}	t_vec3;

/* --- basic arithmetic --- */
t_vec3	vec3_add(t_vec3 a, t_vec3 b);
t_vec3	vec3_sub(t_vec3 a, t_vec3 b);
t_vec3	vec3_scale(t_vec3 a, double t);

/* --- measurement --- */
double	vec3_dot(t_vec3 a, t_vec3 b);
double	vec3_length(t_vec3 a);
double	vec3_length_sq(t_vec3 a);

/* --- direction --- */
t_vec3	vec3_normalize(t_vec3 a);

/* --- cross product (for camera basis vectors) --- */
t_vec3	vec3_cross(t_vec3 a, t_vec3 b);

#endif
