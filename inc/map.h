#ifndef __MAP_H__
#define __MAP_H__

#include "SDL.h"

class Map
{
public:
    Map(int width, int height);
    ~Map();
    SDL_Point CreatFood(void);
    void Clear(void);
    void DrawPoint(int x, int y, uint32_t color);
    void Refresh(void);
    int GetWidth(void){return _width;}
    int GetHeight(void){return _height;}
    uint32_t GetTicks(void);

private:
    int _proportion; // 地图和显示区域的比例 默认为8
    int _width;
    int _height;
private:
    SDL_Window *_sdl_window;
    SDL_Renderer *_sdl_renderer;

    int _window_h;
    int _window_w;
};

#endif /*__MAP_H__*/