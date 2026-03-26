#include <stdio.h>
#include <math.h>
#include "accretion.h"

/*
** Acceptance tests for Workstream D — Accretion Disk.
**
** The acceptance criterion from claude.md is:
**   "Disk is visible as a bright ring around the black hole."
**
** We verify this with unit tests:
**  1. A step that crosses z=0 inside [r_in, r_out] is detected as a hit.
**  2. A step that crosses z=0 outside the annulus is not a hit.
**  3. A step that does NOT cross z=0 is not a hit.
**  4. accretion_color at r_in is hotter (brighter) than at r_out.
**  5. Color at r_in is blue-shifted (more blue) relative to r_out.
*/

static int	test_hit_inside_annulus(void)
{
	double		prev[6];
	double		next[6];
	t_disk_hit	hit;

	/* Step from z = +0.5 to z = -0.5, crossing z=0 at r ≈ 5 (mid-disk) */
	prev[0] = 5.0;  prev[1] = 0.0;  prev[2] =  0.5;
	prev[3] = 0.0;  prev[4] = 0.0;  prev[5] = -1.0;
	next[0] = 5.0;  next[1] = 0.0;  next[2] = -0.5;
	next[3] = 0.0;  next[4] = 0.0;  next[5] = -1.0;

	hit = accretion_check_crossing(prev, next);
	printf("[hit_inside_annulus]  hit=%d  r=%.4f  expected hit=1  ",
		hit.hit, hit.radius);
	if (hit.hit && fabs(hit.radius - 5.0) < 0.01)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

static int	test_no_hit_outside_annulus(void)
{
	double		prev[6];
	double		next[6];
	t_disk_hit	hit;

	/* Crossing at r = 0.5, inside the black hole (r < r_in = 1.5) */
	prev[0] = 0.5;  prev[1] = 0.0;  prev[2] =  0.3;
	next[0] = 0.5;  next[1] = 0.0;  next[2] = -0.3;
	prev[3] = next[3] = prev[4] = next[4] = prev[5] = next[5] = 0.0;

	hit = accretion_check_crossing(prev, next);
	printf("[no_hit_too_inner]    hit=%d  expected hit=0  ", hit.hit);
	if (!hit.hit)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

static int	test_no_crossing(void)
{
	double		prev[6];
	double		next[6];
	t_disk_hit	hit;

	/* Both z positive — no crossing */
	prev[0] = 5.0;  prev[1] = 0.0;  prev[2] = 1.0;
	next[0] = 5.5;  next[1] = 0.0;  next[2] = 0.5;
	prev[3] = next[3] = prev[4] = next[4] = prev[5] = next[5] = 0.0;

	hit = accretion_check_crossing(prev, next);
	printf("[no_crossing]         hit=%d  expected hit=0  ", hit.hit);
	if (!hit.hit)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

static int	test_color_inner_brighter(void)
{
	t_disk_hit	inner;
	t_disk_hit	outer;
	t_color		c_in;
	t_color		c_out;
	double		lum_in;
	double		lum_out;

	inner.hit = 1;  inner.radius = ACCRETION_R_IN;
	outer.hit = 1;  outer.radius = ACCRETION_R_OUT;

	c_in  = accretion_color(inner);
	c_out = accretion_color(outer);

	lum_in  = c_in.r  + c_in.g  + c_in.b;
	lum_out = c_out.r + c_out.g + c_out.b;

	printf("[color_inner_brighter]  lum_in=%.4f  lum_out=%.4f  expected in>out  ",
		lum_in, lum_out);
	if (lum_in > lum_out)
	{
		printf("PASS\n");
		return (1);
	}
	printf("FAIL\n");
	return (0);
}

static int	test_color_inner_bluer(void)
{
	t_disk_hit	inner;
	t_disk_hit	outer;
	t_color		c_in;
	t_color		c_out;

	inner.hit = 1;  inner.radius = ACCRETION_R_IN;
	outer.hit = 1;  outer.radius = ACCRETION_R_OUT;

	c_in  = accretion_color(inner);
	c_out = accretion_color(outer);

	printf("[color_inner_bluer]   b_in=%.4f  b_out=%.4f  expected b_in>b_out  ",
		c_in.b, c_out.b);
	if (c_in.b > c_out.b)
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

	printf("=== Accretion Disk Tests ===\n");
	passed = 0;
	total = 5;
	passed += test_hit_inside_annulus();
	passed += test_no_hit_outside_annulus();
	passed += test_no_crossing();
	passed += test_color_inner_brighter();
	passed += test_color_inner_bluer();
	printf("============================\n");
	printf("Result: %d/%d tests passed\n", passed, total);
	return (passed == total ? 0 : 1);
}
