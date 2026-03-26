#ifndef RK4_H
# define RK4_H

/*
** Generic 4th-order Runge-Kutta integrator.
**
** t_deriv_fn: derivative function pointer.
**   t      - current affine parameter (lambda)
**   state  - current state vector (n elements)
**   deriv  - output: derivative of state at t (n elements, written by f)
**   params - user data passed through unchanged
*/
typedef void	(*t_deriv_fn)(double t, double *state, double *deriv,
				void *params);

/*
** rk4_step: advance state by one step dt.
**   f      - derivative function
**   t      - current parameter value
**   state  - state vector (n elements, updated in place)
**   n      - number of state elements
**   dt     - step size
**   params - forwarded to f unchanged
*/
void	rk4_step(t_deriv_fn f, double t, double *state, int n, double dt,
			void *params);

#endif
