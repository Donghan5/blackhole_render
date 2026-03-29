#include "background.h"
#include <avif/avif.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SKY_PI 3.14159265358979323846

/* Module-internal skybox state — one global image per process. */
static uint8_t	*g_pixels;
static int		g_width;
static int		g_height;

/* Clamp v to [lo, hi]. */
static double	sky_clamp(double v, double lo, double hi)
{
	if (v < lo)
		return (lo);
	if (v > hi)
		return (hi);
	return (v);
}

/*
** libavif 1.x API: avifDecoderReadFile() takes (decoder, image, path).
** We create an avifImage separately, read into it, then destroy the decoder.
** Returns the decoded avifImage on success, NULL on any failure.
** Caller must call avifImageDestroy() on the returned pointer.
*/
static avifImage	*open_image(const char *path)
{
	avifDecoder	*dec;
	avifImage	*image;
	avifResult	res;

	dec = avifDecoderCreate();
	if (!dec)
		return (NULL);
	image = avifImageCreateEmpty();
	if (!image)
	{
		avifDecoderDestroy(dec);
		return (NULL);
	}
	res = avifDecoderReadFile(dec, image, path);
	avifDecoderDestroy(dec);
	if (res != AVIF_RESULT_OK)
	{
		fprintf(stderr, "skybox: cannot read '%s': %s\n",
			path, avifResultToString(res));
		avifImageDestroy(image);
		return (NULL);
	}
	return (image);
}

/*
** Convert a decoded avifImage (YUV) to 8-bit RGB.
** Fills *rgb; caller must call avifRGBImageFreePixels(rgb) on success.
** Returns 1 on success, 0 on failure.
*/
static int	decode_to_rgb(avifImage *image, avifRGBImage *rgb)
{
	avifResult	res;

	memset(rgb, 0, sizeof(*rgb));
	avifRGBImageSetDefaults(rgb, image);
	rgb->format = AVIF_RGB_FORMAT_RGB;
	rgb->depth  = 8;
	if (avifRGBImageAllocatePixels(rgb) != AVIF_RESULT_OK)
		return (0);
	res = avifImageYUVToRGB(image, rgb);
	if (res != AVIF_RESULT_OK)
	{
		fprintf(stderr, "skybox: YUV->RGB failed: %s\n",
			avifResultToString(res));
		avifRGBImageFreePixels(rgb);
		return (0);
	}
	return (1);
}

/*
** Copy the decoded RGB pixels into a private malloc'd buffer (g_pixels).
** Sets g_width and g_height as a side-effect.
** Returns 1 on success, 0 on malloc failure.
*/
static int	store_pixels(const avifRGBImage *rgb)
{
	size_t	bytes;

	g_width  = (int)rgb->width;
	g_height = (int)rgb->height;
	bytes    = (size_t)(g_width * g_height * 3);
	g_pixels = malloc(bytes);
	if (!g_pixels)
	{
		fprintf(stderr, "skybox: out of memory (%zu bytes)\n", bytes);
		return (0);
	}
	memcpy(g_pixels, rgb->pixels, bytes);
	return (1);
}

int	skybox_init(const char *path)
{
	avifImage	*image;
	avifRGBImage	rgb;
	int				ok;

	image = open_image(path);
	if (!image)
		return (0);
	ok = decode_to_rgb(image, &rgb);
	avifImageDestroy(image);
	if (ok)
	{
		ok = store_pixels(&rgb);
		avifRGBImageFreePixels(&rgb);
	}
	return (ok);
}

void	skybox_destroy(void)
{
	free(g_pixels);
	g_pixels = NULL;
	g_width  = 0;
	g_height = 0;
}

/*
** Fetch one texel from g_pixels at normalised coordinates (u, v) ∈ [0,1]².
** Wraps horizontally; clamps vertically (poles).
*/
static t_color	sample_pixel(double u, double v)
{
	int		px;
	int		py;
	int		idx;
	t_color	c;

	px  = (int)(u * g_width) % g_width;
	py  = (int)(sky_clamp(v * g_height, 0.0, (double)(g_height - 1)));
	idx = (py * g_width + px) * 3;
	c.r = g_pixels[idx]     / 255.0;
	c.g = g_pixels[idx + 1] / 255.0;
	c.b = g_pixels[idx + 2] / 255.0;
	return (c);
}

/*
** Map an escape-ray direction to an equirectangular texel.
**
** Coordinate convention (right-handed):
**   +Y = north pole,  xz-plane = equator
**   theta = polar angle from +Y  ∈ [0, π]
**   phi   = azimuthal angle in xz ∈ (-π, π]  (atan2(x, z))
**   u = (phi + π) / 2π  → [0, 1]  (wraps east–west)
**   v = theta / π       → [0, 1]  (0 = north, 1 = south)
*/
t_color	background_color(t_ray ray)
{
	t_vec3	d;
	double	theta;
	double	phi;
	double	u;
	double	v;

	if (!g_pixels)
		return ((t_color){0.0, 0.0, 0.0});
	d     = ray.direction;   /* normalised by make_escape_ray / camera */
	theta = acos(sky_clamp(d.y, -1.0, 1.0));  /* polar angle from +Y */
	phi   = atan2(d.x, d.z);                  /* azimuth in xz-plane  */
	u     = (phi + SKY_PI) / (2.0 * SKY_PI);
	v     = theta / SKY_PI;
	return (sample_pixel(u, v));
}

/* Convert floating-point color in [0,1] to a packed SDL Uint32 pixel */
Uint32	color_to_sdl(t_color color, SDL_PixelFormat *format)
{
	Uint8	r;
	Uint8	g;
	Uint8	b;

	r = (Uint8)(color.r * 255.999);
	g = (Uint8)(color.g * 255.999);
	b = (Uint8)(color.b * 255.999);
	return (SDL_MapRGB(format, r, g, b));
}
