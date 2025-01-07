#include "map.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iostream>
using namespace std;

Map::Map(int width, int height)
{
    _width = width;
    _height = height;
    _proportion = 8;

    _window_w = _width * _proportion;
    _window_h = _height * _proportion;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize." << endl;
        cerr << "SDL_Error: " << SDL_GetError() << endl;
    }

    // CreateWindows
    _sdl_window = SDL_CreateWindow("test title", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, _window_w,
                                _window_h, SDL_WINDOW_SHOWN);
    if(nullptr == _sdl_window)
    {
        cerr << "SDL_CreateWindow filed" << endl;
        cerr << "sdl err: " << SDL_GetError() << endl;
    }

    // CreateRenderer
    _sdl_renderer = SDL_CreateRenderer(_sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if(nullptr == _sdl_renderer)
    {
        cerr << "SDL_CreateRenderer filed" << endl;
        cerr << "sdl err: " << SDL_GetError() << endl;
    }
    SDL_SetRenderDrawColor(_sdl_renderer, 0x00, 0x00, 0x00, 0x00);
}

Map::~Map()
{
    SDL_DestroyWindow(_sdl_window);
    SDL_DestroyRenderer(_sdl_renderer);
    SDL_Quit();
}

SDL_Point Map::CreatFood(void)
{
    SDL_Point food;

    srand (time(0));
    food.x = rand() % _width;
    food.y = rand() % _height;

    return food;
}

void Map::Clear(void)
{
    if(_sdl_renderer)    
        SDL_RenderClear(_sdl_renderer);
}

void Map::DrawPoint(int x, int y, uint32_t color)
{
    SDL_Rect rect;
    rect.x = x * _proportion;
    rect.y = y * _proportion;
    rect.w = rect.h = _proportion;
    uint8_t r, g, b, a;

    SDL_GetRenderDrawColor(_sdl_renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(_sdl_renderer, (color>>16)&0xff, (color>>8)&0xff, (color)&0xff, 0);
    SDL_RenderFillRect(_sdl_renderer, &rect);
    SDL_SetRenderDrawColor(_sdl_renderer, r, g, b, a);
}

void Map::Refresh(void)
{
    SDL_RenderPresent(_sdl_renderer);
}

uint32_t Map::GetTicks(void)
{
    return SDL_GetTicks();
}
