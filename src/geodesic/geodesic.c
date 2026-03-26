#include <math.h>
#include <string.h>
#include "geodesic.h"

/*
** Schwarzschild null geodesic acceleration in Cartesian-ish coordinates.
**
** For a null ray in Schwarzschild spacetime (G=c=1, rs = 2M), the geodesic
** equation in Cartesian coordinates x^i (where r = |x⃗|) reduces to:
**
**   d²x^i / dλ² = -(3 * rs * L²) / (2 * r⁵) * x^i
**
** where L² = |x⃗ × v⃗|² is the squared angular momentum of the ray and
** λ is the affine parameter.
**
** Derivation: from the equatorial-plane geodesic equations
**   r̈ - r φ̇² = -(3 * rs * L²) / (2 * r⁴)
** combined with d²x^i/dλ² = (r̈ - rφ̇²)/r * x^i for central-force motion.
** The null constraint eliminates E so the result depends only on L and r.
**
** For L = 0 (radial photon), the acceleration vanishes and the photon
** travels in a straight line — this is correct for the affine parameter:
** radial null geodesics satisfy dr/dλ = const, so r̈ = 0. ✓
**
** Weak-field deflection check (L=b, r >> rs):
**   Δφ = ∫ a_y dλ ≈ -3rsb³/2 ∫ (x²+b²)^{-5/2} dx
**       = -3rsb³/2 * 4/(3b⁴) = -2rs/b  ✓  (matches James et al. 2015)
*/
static void	schwarzschild_accel(double *pos, double *vel, double *accel)
{
	double	cross_x;
	double	cross_y;
	double	cross_z;
	double	L2;
	double	r2;
	double	r;
	double	factor;
	int		i;

	/* L² = |x⃗ × v⃗|² */
	cross_x = pos[1] * vel[2] - pos[2] * vel[1];
	cross_y = pos[2] * vel[0] - pos[0] * vel[2];
	cross_z = pos[0] * vel[1] - pos[1] * vel[0];
	L2 = cross_x * cross_x + cross_y * cross_y + cross_z * cross_z;

	r2 = pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
	r = sqrt(r2);

	if (r < 1e-10)
	{
		accel[0] = 0.0;
		accel[1] = 0.0;
		accel[2] = 0.0;
		return ;
	}

	/* factor = -3 * rs * L² / (2 * r⁵) */
	factor = -3.0 * GEODESIC_RS * L2 / (2.0 * r2 * r2 * r);

	i = 0;
	while (i < 3)
	{
		accel[i] = factor * pos[i];
		i++;
	}
}

/*
** geodesic_deriv: RK4-compatible derivative for null geodesics.
** state[0..2] = position (x, y, z)
** state[3..5] = velocity (vx, vy, vz) = dx^i/dλ
*/
void	geodesic_deriv(double t, double *state, double *deriv, void *params)
{
	double	accel[3];

	(void)t;
	(void)params;

	deriv[0] = state[3];
	deriv[1] = state[4];
	deriv[2] = state[5];

	schwarzschild_accel(state, state + 3, accel);
	deriv[3] = accel[0];
	deriv[4] = accel[1];
	deriv[5] = accel[2];
}

/*
** geodesic_trace: integrate a null geodesic until absorbed or escaped.
** Returns GEODESIC_ABSORBED (r < rs/2) or GEODESIC_ESCAPED (r > R_MAX).
** final_state receives the last integrated state.
*/
t_geodesic_result	geodesic_trace(double *initial, double dt, int max_steps,
						double *final_state)
{
	double	state[6];
	double	r2;
	double	lambda;
	int		step;

	memcpy(state, initial, 6 * sizeof(double));
	lambda = 0.0;
	step = 0;
	while (step < max_steps)
	{
		r2 = state[0] * state[0] + state[1] * state[1] + state[2] * state[2];

		if (r2 < GEODESIC_RS_HALF * GEODESIC_RS_HALF)
		{
			memcpy(final_state, state, 6 * sizeof(double));
			return (GEODESIC_ABSORBED);
		}
		if (r2 > GEODESIC_R_MAX * GEODESIC_R_MAX)
		{
			memcpy(final_state, state, 6 * sizeof(double));
			return (GEODESIC_ESCAPED);
		}
		rk4_step(geodesic_deriv, lambda, state, 6, dt, NULL);
		lambda += dt;
		step++;
	}
	memcpy(final_state, state, 6 * sizeof(double));
	return (GEODESIC_ESCAPED);
}
