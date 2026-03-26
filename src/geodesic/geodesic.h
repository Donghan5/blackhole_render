#ifndef GEODESIC_H
# define GEODESIC_H

# include "../rk4/rk4.h"

/* Schwarzschild radius in geometrized units (G=c=M=1 → rs=2) */
# define GEODESIC_RS    1.0
/* Photon absorbed if r < RS_HALF */
# define GEODESIC_RS_HALF  (GEODESIC_RS * 0.5)
/* Photon escaped if r > R_MAX */
# define GEODESIC_R_MAX 50.0

typedef enum e_geodesic_result
{
	GEODESIC_ABSORBED,
	GEODESIC_ESCAPED,
}	t_geodesic_result;

/*
** Schwarzschild geodesic derivative for a null ray.
** state = [x, y, z, px, py, pz]
** Implements:
**   dx^i/dλ = p^i
**   dp^i/dλ = -Γ^i_μν p^μ p^ν
*/
void				geodesic_deriv(double t, double *state, double *deriv,
						void *params);

/*
** Trace a photon from initial state until absorbed or escaped.
** Returns GEODESIC_ABSORBED or GEODESIC_ESCAPED.
** final_state is written with the last integration state.
*/
t_geodesic_result	geodesic_trace(double *initial, double dt, int max_steps,
						double *final_state);

#endif
