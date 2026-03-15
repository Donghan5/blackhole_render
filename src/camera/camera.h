#ifndef CAMERA_H
# define CAMERA_H

# include "../math/vec3.h"
# include "../ray/ray.h"

typedef struct s_camera
{
	t_vec3	position;
	t_vec3	lower_left_corner;
	t_vec3	horizontal;	/* full-width vector along the viewport */
	t_vec3	vertical;	/* full-height vector along the viewport */
	int		width;
	int		height;
}	t_camera;

t_camera	camera_init(t_vec3 position, t_vec3 target, t_vec3 up,
				double vfov_deg, int width, int height);
t_ray		camera_get_ray(t_camera camera, int px, int py);

#endif
