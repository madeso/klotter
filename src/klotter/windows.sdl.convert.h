#pragma once



#include "klotter/dependency_sdl.h"
#include "klotter/input/key.h"
#include "klotter/input/axis.h"


namespace sdl
{
    glm::ivec2 get_hat_values(Uint8 hat);

    klotter::KeyboardKey to_keyboard_key(SDL_Keysym key);

    klotter::MouseButton to_mouse_button(Uint8 mb);

    klotter::GamecontrollerButton to_controller_button(SDL_GameControllerButton b);
    klotter::GamecontrollerButton to_controller_button(SDL_GameControllerAxis b);
    klotter::GamecontrollerAxis to_controller_axis(SDL_GameControllerAxis b);
}

