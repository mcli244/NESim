#include "input.h"
#include <iostream>

using namespace std;

SDL_Keycode Input::HandleInput(void){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
        return SDLK_ESCAPE;
    } else if (e.type == SDL_KEYDOWN) {
      return e.key.keysym.sym;
    }
  }
  return 0;
}










