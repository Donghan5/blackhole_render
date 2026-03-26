#include <stdio.h>
#include <math.h>
#include "rk4.h"

/*
** Acceptance test from claude.md Workstream B:
**   A particle in freefall matches the analytic solution y = -0.5*g*t² to
**   high accuracy.
**
** State layout: [y, vy]  (1D freefall along y-axis)
** Derivative:   dy/dt  = vy
**               dvy/dt = -g
*/

#define GRAVITY 9.81
#define DT      0.001
#define T_END   2.0

static void	freefall_deriv(double t, double *state, double *deriv, void *params)
{
	(void)t;
	(void)params;
	deriv[0] = state[1];    /* dy/dt  = vy       */
	deriv[1] = -GRAVITY;    /* dvy/dt = -g       */
}

static int	test_freefall(void)
{
	double	state[2];
	double	t;
	double	y_analytic;
	double	err;
	int		steps;

	state[0] = 0.0;   /* initial y  = 0    */
	state[1] = 0.0;   /* initial vy = 0    */
	t = 0.0;
	steps = (int)(T_END / DT);

	while (steps-- > 0)
	{
		rk4_step(freefall_deriv, t, state, 2, DT, NULL);
		t += DT;
	}

	y_analytic = -0.5 * GRAVITY * T_END * T_END;
	err = fabs(state[0] - y_analytic) / fabs(y_analytic);

	printf("[freefall]  y_sim=%.8f  y_analytic=%.8f  rel_err=%.2e  ",
		state[0], y_analytic, err);
	if (err < 1e-8)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

int	main(void)
{
	int	passed;

	printf("=== RK4 Integrator Tests ===\n");
	passed = test_freefall();
	printf("============================\n");
	printf("Result: %d/1 tests passed\n", passed);
	return (passed == 1 ? 0 : 1);
}
