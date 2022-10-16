#pragma once



#include "three/dependency_sdl.h"
#include "three/input/key.h"
#include "three/input/axis.h"


namespace sdl
{
    glm::ivec2 get_hat_values(Uint8 hat);

    three::KeyboardKey to_keyboard_key(SDL_Keysym key);

    three::MouseButton to_mouse_button(Uint8 mb);

    three::GamecontrollerButton to_controller_button(SDL_GameControllerButton b);
    three::GamecontrollerButton to_controller_button(SDL_GameControllerAxis b);
    three::GamecontrollerAxis to_controller_axis(SDL_GameControllerAxis b);
}

