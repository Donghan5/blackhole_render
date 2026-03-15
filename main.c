#include <SDL2/SDL.h>

#define WIDTH  800
#define HEIGHT 600

void    put_pixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
    Uint32  *pixels = (Uint32 *)surface->pixels;
    pixels[y * surface->w + x] = color;
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window  *window  = SDL_CreateWindow("blackhole",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WIDTH, HEIGHT, 0);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
        {
            Uint32 color = SDL_MapRGB(surface->format, x % 255, y % 255, 100);
            put_pixel(surface, x, y, color);
        }

    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    while (SDL_WaitEvent(&e))
        if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
            break;

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}