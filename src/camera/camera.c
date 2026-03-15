#include <math.h>
#include "camera.h"

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/* Build an orthonormal basis: forward, right, cam_up from world-space inputs */
static void	build_basis(t_vec3 pos, t_vec3 target, t_vec3 up,
				t_vec3 *right, t_vec3 *cam_up, t_vec3 *forward)
{
	*forward = vec3_normalize(vec3_sub(target, pos));
	*right   = vec3_normalize(vec3_cross(*forward, up));
	*cam_up  = vec3_cross(*right, *forward);
}

/* Lower-left world-space corner of the viewport (focal length = 1) */
static t_vec3	viewport_corner(t_vec3 pos, t_vec3 forward, t_vec3 right,
					t_vec3 cam_up, double hw, double hh)
{
	t_vec3	center;
	t_vec3	offset;

	center = vec3_add(pos, forward);
	offset = vec3_add(vec3_scale(right, hw), vec3_scale(cam_up, hh));
	return (vec3_sub(center, offset));
}

t_camera	camera_init(t_vec3 pos, t_vec3 target, t_vec3 up,
				double vfov_deg, int width, int height)
{
	t_camera	cam;
	t_vec3		right;
	t_vec3		cam_up;
	t_vec3		forward;
	double		hh;
	double		hw;

	build_basis(pos, target, up, &right, &cam_up, &forward);
	/* hh: half-height of the viewport at distance 1 */
	hh               = tan((vfov_deg * M_PI / 180.0) / 2.0);
	hw               = hh * ((double)width / (double)height);
	cam.position     = pos;
	cam.width        = width;
	cam.height       = height;
	cam.horizontal   = vec3_scale(right, 2.0 * hw);
	cam.vertical     = vec3_scale(cam_up, 2.0 * hh);
	cam.lower_left_corner = viewport_corner(pos, forward, right, cam_up, hw, hh);
	return (cam);
}

t_ray	camera_get_ray(t_camera camera, int px, int py)
{
	double	u;
	double	v;
	t_vec3	target;
	t_ray	ray;

	/* u in [0,1] left→right, v in [0,1] bottom→top */
	u = (double)px / (double)(camera.width - 1);
	v = (double)(camera.height - 1 - py) / (double)(camera.height - 1);
	target        = vec3_add(
		vec3_add(camera.lower_left_corner, vec3_scale(camera.horizontal, u)),
		vec3_scale(camera.vertical, v));
	ray.origin    = camera.position;
	ray.direction = vec3_normalize(vec3_sub(target, camera.position));
	return (ray);
}
