#ifndef ACCRETION_H
# define ACCRETION_H

# include "../background/background.h"

/* Disk geometry — equatorial plane (z = 0) */
# define ACCRETION_R_IN   1.5    /* inner edge: 3rs/2 (photon sphere), rs=1 */
# define ACCRETION_R_OUT  10.0   /* outer edge */

/*
** t_disk_hit: result of disk intersection check.
**   hit    - 1 if the ray crossed the disk plane in the valid annulus
**   radius - radial distance at the crossing point (if hit)
*/
typedef struct s_disk_hit
{
	int		hit;
	double	radius;
}	t_disk_hit;

/*
** accretion_check_crossing: test whether a ray step crosses z=0 in the disk.
**
** prev_state[0..5] — state vector before RK4 step [x,y,z,vx,vy,vz]
** next_state[0..5] — state vector after  RK4 step
**
** Returns a t_disk_hit describing the first equatorial crossing in [r_in, r_out].
** Uses linear interpolation in z to find the crossing point.
*/
t_disk_hit	accretion_check_crossing(double *prev_state, double *next_state);

/*
** accretion_color: map a disk hit to an RGB color.
**
** Temperature model: T ∝ r^(-3/4)  (thin-disk profile)
** The temperature is mapped to a blackbody RGB via a simplified Wien peak:
**   T_ref = temperature at r_in  (hottest, blue-white)
**   T at r → cooler toward outer edge (orange-red)
*/
t_color		accretion_color(t_disk_hit hit);

#endif
