# Black Hole Renderer — CLAUDE.md

## Project Overview

A physically-based black hole renderer written in C using SDL2 for display only.
Everything else (math, ray tracing, RK4 integrator, geodesic solver) is built from scratch.

Target: Schwarzschild black hole with gravitational lensing via ray tracing + RK4 numerical integration.

---

## Tech Stack

- **Language**: C (C99)
- **Rendering**: SDL2 (window + pixel buffer only)
- **Math**: Custom vec3, mat4 — no external math library
- **Build**: Makefile

---

## Project Structure

```
blackhole/
├── CLAUDE.md
├── Makefile
├── src/
│   ├── main.c
│   ├── camera/
│   │   ├── camera.c
│   │   └── camera.h
│   ├── ray/
│   │   ├── ray.c
│   │   └── ray.h
│   ├── math/
│   │   ├── vec3.c
│   │   └── vec3.h
│   ├── rk4/
│   │   ├── rk4.c
│   │   └── rk4.h
│   ├── geodesic/
│   │   ├── geodesic.c
│   │   └── geodesic.h
│   ├── background/
│   │   ├── background.c
│   │   └── background.h
│   └── render/
│       ├── render.c
│       └── render.h
└── assets/
    └── skybox.png  (optional)
```

---

## Build Order & Parallel Workstreams

This project is designed to be developed in parallel across multiple Claude Code instances.
Each workstream is **independent** and can be built and tested in isolation.

### Phase 1 — Foundation (sequential, do this first)

**Workstream A: Math library + Camera + Ray + Background**

Must be completed before anything else. This is the backbone.

1. `src/math/vec3` — 3D vector math
2. `src/camera/` — camera setup, ray generation
3. `src/ray/` — ray struct and utilities
4. `src/background/` — skybox / gradient background color
5. `src/main.c` + `src/render/` — SDL2 window, pixel loop, render each ray to background color

**Acceptance test**: Window opens, renders a smooth gradient or solid background color based on ray direction. No black hole yet.

---

### Phase 2 — Parallel workstreams (after Phase 1)

Launch these simultaneously in separate Claude Code instances.

---

**Workstream B: RK4 Integrator** (`src/rk4/`)

Implement a generic RK4 ODE solver that works on a state vector.

- State vector: `double state[6]` → `[x, y, z, px, py, pz]` (position + momentum)
- Interface:
```c
typedef void (*t_deriv_fn)(double t, double *state, double *deriv, void *params);

void rk4_step(t_deriv_fn f, double t, double *state, int n, double dt, void *params);
```
- The `f` function pointer is swappable — this is intentional
- Test with simple freefall or circular orbit before plugging in geodesic

**Acceptance test**: A particle in freefall matches the analytic solution `y = -0.5 * g * t²` to high accuracy.

---

**Workstream C: Geodesic Equation** (`src/geodesic/`)

Implement the Schwarzschild geodesic equations for photon trajectories.

Reference: James et al. 2015 (Interstellar paper), Section 3.

The equations in Hamiltonian form:

```
dx/dλ = ∂H/∂p
dp/dλ = -∂H/∂x
```

For Schwarzschild metric (G=c=1, M=1):

```
H = (1/2) * g^μν * p_μ * p_ν = 0   (null geodesic for photons)
```

In Cartesian-like coordinates (to avoid coordinate singularities):

```
d²xⁱ/dλ² = -Γⁱ_μν * (dxᵐ/dλ) * (dxⁿ/dλ)
```

Use the following simplified form for implementation:

```c
// state = [x, y, z, px, py, pz]
// r = sqrt(x² + y² + z²)
// rs = Schwarzschild radius = 2GM/c² (set to 1.0 in natural units)

void geodesic_deriv(double t, double *state, double *deriv, void *params);
```

- Termination condition 1: `r < rs * 0.5` → photon absorbed (black)
- Termination condition 2: `r > r_max` → photon escaped (sample background)

**Acceptance test**: A photon shot directly at the black hole gets absorbed. A photon shot far away escapes with slight deflection matching the analytic weak-field approximation.

---

**Workstream D: Accretion Disk** (`src/accretion/`) *(optional, do last)*

A flat disk in the equatorial plane (z = 0) with inner radius `r_in` and outer radius `r_out`.

- Check if ray crosses z=0 plane during integration
- If crossing point satisfies `r_in < r < r_out` → color based on temperature gradient
- Temperature model: `T ∝ r^(-3/4)` → map to RGB via blackbody approximation

**Acceptance test**: Disk is visible as a bright ring around the black hole.

---

## Integration Plan (after Phase 2)

Once all workstreams are done, integrate in `src/render/render.c`:

```c
for each pixel (x, y):
    ray    = camera_get_ray(camera, x, y)
    color  = trace_ray(ray)          // runs RK4 + geodesic
    put_pixel(surface, x, y, color)
```

`trace_ray()`:
1. Init state vector from ray origin + direction
2. Loop: `rk4_step(geodesic_deriv, ...)`
3. Check termination conditions each step
4. Return color based on outcome (black / background / disk)

---

## Constants & Units

Use geometrized units: `G = c = 1`, black hole mass `M = 1`

```c
#define RS      1.0     // Schwarzschild radius (2GM/c²)
#define R_MAX   50.0    // max ray travel distance
#define DT      0.1     // RK4 step size (tune for accuracy vs speed)
#define WIDTH   800
#define HEIGHT  600
```

---

## Coding Standards

- No global variables except constants
- Every module has its own `.h` with clear interface
- `f()` (derivative function) is always passed as a function pointer — never hardcoded
- All vec3 operations return by value or write to pointer — no hidden allocation
- No malloc unless absolutely necessary

### Function Design
- **Maximum 25 lines per function** — hard limit, no exceptions
- If a function exceeds 25 lines, split it into meaningful sub-functions with clear names
- Every function must do **one thing only** — name it after that one thing
- Prefer many small, well-named functions over few large ones

### Readability
- Write code as if the reader has never seen the project before
- Variable and function names must be self-explanatory — no single-letter names except loop indices (`i`, `j`, `x`, `y`)
- No clever tricks that sacrifice clarity
- Every non-obvious formula must have a comment explaining what it represents physically

### Resource Management
- Every `malloc` must have a corresponding `free` in a clearly defined owner
- Every SDL resource (`SDL_Window`, `SDL_Surface`, etc.) must be destroyed before `SDL_Quit()`
- No resource is allocated without an explicit cleanup path
- On error: free everything acquired so far, then exit cleanly — no leaks on failure paths
- Prefer stack allocation over heap whenever possible

---

## Key References

- James et al. 2015 — "Gravitational lensing by spinning black holes in astrophysics, and in the movie Interstellar"
- "Ray Tracing in One Weekend" — baseline ray tracer structure
- Wikipedia: Schwarzschild geodesics
