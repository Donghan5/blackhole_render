#include <stdlib.h>
#include <string.h>
#include "rk4.h"

/*
** Compute a weighted slope: slope = f(t + dt_frac, state + dt_frac * prev_k)
** Writes the result into k_out (n elements).
*/
static void	compute_slope(t_deriv_fn f, double t, double *state,
				double *prev_k, int n, double dt_frac, double *k_out,
				void *params)
{
	double	tmp[n];
	int		i;

	i = 0;
	while (i < n)
	{
		tmp[i] = state[i] + dt_frac * prev_k[i];
		i++;
	}
	f(t + dt_frac, tmp, k_out, params);
}

/*
** rk4_step: one RK4 advance.
** Classic 4-stage formula:
**   k1 = f(t,        state)
**   k2 = f(t + h/2,  state + h/2 * k1)
**   k3 = f(t + h/2,  state + h/2 * k2)
**   k4 = f(t + h,    state + h   * k3)
**   state += h/6 * (k1 + 2*k2 + 2*k3 + k4)
*/
void	rk4_step(t_deriv_fn f, double t, double *state, int n, double dt,
			void *params)
{
	double	k1[n];
	double	k2[n];
	double	k3[n];
	double	k4[n];
	double	zero[n];
	int		i;

	memset(zero, 0, sizeof(double) * n);

	f(t, state, k1, params);
	compute_slope(f, t, state, k1, n, dt * 0.5, k2, params);
	compute_slope(f, t, state, k2, n, dt * 0.5, k3, params);
	compute_slope(f, t, state, k3, n, dt,        k4, params);

	i = 0;
	while (i < n)
	{
		state[i] += (dt / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
		i++;
	}
}
