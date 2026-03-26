#include <stdio.h>
#include <math.h>
#include "geodesic.h"

/*
** Acceptance tests from claude.md Workstream C:
**
**  1. A photon aimed directly at the black hole is absorbed.
**  2. A photon shot far away (large impact parameter b) escapes with a
**     small deflection angle matching the weak-field analytic result:
**
**       Δφ ≈ 2 * rs / b   (radians, weak-field Schwarzschild)
**
** State layout: [x, y, z, px, py, pz]
*/

#define DT          0.05
#define MAX_STEPS   100000

/*
** Test 1 — Direct impact.
** Photon starts at (10, 0, 0) aimed toward the origin (direction -x).
** Impact parameter b = 0, so it must be absorbed.
*/
static int	test_direct_impact(void)
{
	double				initial[6];
	double				final[6];
	t_geodesic_result	result;

	/* position: (10, 0, 0), momentum: (-1, 0, 0) */
	initial[0] = 10.0;
	initial[1] = 0.0;
	initial[2] = 0.0;
	initial[3] = -1.0;
	initial[4] = 0.0;
	initial[5] = 0.0;

	result = geodesic_trace(initial, DT, MAX_STEPS, final);
	printf("[direct_impact]    result=%s  expected=ABSORBED  ",
		result == GEODESIC_ABSORBED ? "ABSORBED" : "ESCAPED");
	if (result == GEODESIC_ABSORBED)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

/*
** Test 2 — Off-axis photon, small deflection.
** Photon starts at (-30, b, 0) moving in +x direction.
** For b >> rs the weak-field deflection angle is:
**   Δφ = 2 * rs / b
**
** We compare the simulated deflection with the analytic prediction
** and require relative error < 20% (generous for the discrete step size).
**
** Deflection angle from simulation:
**   The final momentum direction is (px, py, 0).
**   Δφ_sim = atan2(|py|, px)   (small-angle regime)
*/
static int	test_weak_field_deflection(void)
{
	double				initial[6];
	double				final_state[6];
	t_geodesic_result	result;
	double				b;
	double				phi_analytic;
	double				phi_sim;
	double				rel_error;
	double				px;
	double				py;

	b = 10.0;   /* impact parameter, units of rs */

	/* Start far to the left, offset b in y, moving in +x */
	initial[0] = -30.0;
	initial[1] = b;
	initial[2] = 0.0;
	/* normalised momentum in +x direction */
	initial[3] = 1.0;
	initial[4] = 0.0;
	initial[5] = 0.0;

	result = geodesic_trace(initial, DT, MAX_STEPS, final_state);

	printf("[weak_deflection]  result=%s  expected=ESCAPED  ",
		result == GEODESIC_ESCAPED ? "ESCAPED" : "ABSORBED");
	if (result != GEODESIC_ESCAPED)
	{
		printf("FAIL (photon absorbed unexpectedly)\n");
		return (0);
	}

	px = final_state[3];
	py = final_state[4];

	/*
	** The photon is deflected toward the black hole (negative y offset).
	** Deflection angle = angle between final momentum and initial (+x) direction.
	*/
	phi_sim = fabs(atan2(fabs(py), px));
	phi_analytic = 2.0 * GEODESIC_RS / b;  /* weak-field formula */

	rel_error = fabs(phi_sim - phi_analytic) / phi_analytic;

	printf("\n  phi_sim=%.6f rad  phi_analytic=%.6f rad  rel_error=%.2f%%  ",
		phi_sim, phi_analytic, rel_error * 100.0);
	if (rel_error < 0.20)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL (relative error > 20%%)\n");
	return (0);
}

/*
** Test 3 — Photon well outside r_max.
** A photon already at r = 40 (< r_max=50) moving outward should escape.
*/
static int	test_photon_escapes(void)
{
	double				initial[6];
	double				final_state[6];
	t_geodesic_result	result;

	/* position: (40, 5, 0), momentum pointing outward */
	initial[0] = 40.0;
	initial[1] = 5.0;
	initial[2] = 0.0;
	initial[3] = 1.0;
	initial[4] = 0.1;
	initial[5] = 0.0;

	result = geodesic_trace(initial, DT, MAX_STEPS, final_state);
	printf("[photon_escapes]   result=%s  expected=ESCAPED   ",
		result == GEODESIC_ESCAPED ? "ESCAPED" : "ABSORBED");
	if (result == GEODESIC_ESCAPED)
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
	int	total;

	printf("=== Geodesic Equation Tests ===\n");
	passed = 0;
	total = 3;
	passed += test_direct_impact();
	passed += test_weak_field_deflection();
	passed += test_photon_escapes();
	printf("===============================\n");
	printf("Result: %d/%d tests passed\n", passed, total);
	if (passed == total)
		return (0);
	return (1);
}
