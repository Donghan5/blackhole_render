#include <math.h>
#include "accretion.h"

/*
** linear_interp_z_crossing: find the fraction t∈[0,1] where z passes through 0.
**   z0 + t*(z1 - z0) = 0  =>  t = -z0 / (z1 - z0)
** Returns -1.0 if there is no crossing (same sign or one is zero handled below).
*/
static double	z_crossing_fraction(double z0, double z1)
{
	double	dz;

	dz = z1 - z0;
	if (fabs(dz) < 1e-15)
		return (-1.0);
	return (-z0 / dz);
}

/*
** Interpolate a position component at fraction t between prev and next states.
*/
static double	lerp(double a, double b, double t)
{
	return (a + t * (b - a));
}

/*
** accretion_check_crossing: detect equatorial plane (z=0) crossing.
**
** A sign change in z between prev and next indicates a crossing.
** We linearly interpolate to find the crossing radius.
*/
t_disk_hit	accretion_check_crossing(double *prev_state, double *next_state)
{
	t_disk_hit	result;
	double		z0;
	double		z1;
	double		frac;
	double		x_cross;
	double		y_cross;
	double		r_cross;

	result.hit = 0;
	result.radius = 0.0;

	z0 = prev_state[2];
	z1 = next_state[2];

	/* Require a sign change (actual crossing, not tangent touch) */
	if (z0 * z1 >= 0.0)
		return (result);

	frac = z_crossing_fraction(z0, z1);
	if (frac < 0.0 || frac > 1.0)
		return (result);

	x_cross = lerp(prev_state[0], next_state[0], frac);
	y_cross = lerp(prev_state[1], next_state[1], frac);
	r_cross = sqrt(x_cross * x_cross + y_cross * y_cross);

	if (r_cross < ACCRETION_R_IN || r_cross > ACCRETION_R_OUT)
		return (result);

	result.hit = 1;
	result.radius = r_cross;
	return (result);
}

/*
** blackbody_rgb: approximate RGB for a blackbody at normalized temperature u.
**
** u = T / T_max ∈ (0, 1]:
**   u close to 1 (hot, inner disk) → blue-white
**   u small      (cool, outer disk) → orange-red
**
** This is a hand-tuned piecewise approximation to the Wien RGB curve,
** sufficient for visualization purposes.
*/
static t_color	blackbody_rgb(double u)
{
	t_color	c;

	/* Red: rises quickly from low temperature, saturates early */
	c.r = fmin(1.0, 1.8 * u);

	/* Green: peaks around mid temperature */
	c.g = fmin(1.0, 2.0 * u * u + 0.3 * u);

	/* Blue: only present at high temperature */
	c.b = fmin(1.0, 3.0 * u * u * u);

	return (c);
}

/*
** accretion_color: convert a disk intersection to a color.
**
** Temperature T ∝ r^(-3/4).
** Normalize so that r_in is the hottest (u = 1) and r_out is coolest.
**
** T(r)     = r^(-3/4)
** T_in     = r_in^(-3/4)
** T_out    = r_out^(-3/4)
** u(r)     = (T(r) - T_out) / (T_in - T_out)
*/
t_color		accretion_color(t_disk_hit hit)
{
	double	t_r;
	double	t_in;
	double	t_out;
	double	u;

	t_r   = pow(hit.radius,          -0.75);
	t_in  = pow(ACCRETION_R_IN,      -0.75);
	t_out = pow(ACCRETION_R_OUT,     -0.75);

	u = (t_r - t_out) / (t_in - t_out);
	if (u < 0.0)
		u = 0.0;
	if (u > 1.0)
		u = 1.0;

	return (blackbody_rgb(u));
}
