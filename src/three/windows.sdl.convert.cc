#include "three/windows.sdl.convert.h"

#include "three/assert.h"


namespace sdl
{


glm::ivec2 get_hat_values(Uint8 hat)
{
    switch (hat)
    {
        case SDL_HAT_LEFTUP: return glm::ivec2(-1, 1);
        case SDL_HAT_UP: return glm::ivec2(0, 1);
        case SDL_HAT_RIGHTUP: return glm::ivec2(1, 1);
        case SDL_HAT_LEFT: return glm::ivec2(-1, 0);
        case SDL_HAT_CENTERED: return glm::ivec2(0, 0);
        case SDL_HAT_RIGHT: return glm::ivec2(1, 0);
        case SDL_HAT_LEFTDOWN: return glm::ivec2(-1, -1);
        case SDL_HAT_DOWN: return glm::ivec2(0, -1);
        case SDL_HAT_RIGHTDOWN: return glm::ivec2(1, -1);
        default:
            DIE("Invalid hat value");
            return glm::ivec2(0, 0);
    }
}


three::KeyboardKey to_keyboard_key(SDL_Keysym key)
{
    switch (key.sym)
    {
        case SDLK_RETURN: return three::KeyboardKey::return_key;
        case SDLK_ESCAPE: return three::KeyboardKey::escape;
        case SDLK_BACKSPACE: return three::KeyboardKey::backspace;
        case SDLK_TAB: return three::KeyboardKey::tab;
        case SDLK_SPACE: return three::KeyboardKey::space;
        case SDLK_EXCLAIM: return three::KeyboardKey::exclaim;
        case SDLK_QUOTEDBL: return three::KeyboardKey::quotedbl;
        case SDLK_HASH: return three::KeyboardKey::hash;
        case SDLK_PERCENT: return three::KeyboardKey::percent;
        case SDLK_DOLLAR: return three::KeyboardKey::dollar;
        case SDLK_AMPERSAND: return three::KeyboardKey::ampersand;
        case SDLK_QUOTE: return three::KeyboardKey::quote;
        case SDLK_LEFTPAREN: return three::KeyboardKey::left_paren;
        case SDLK_RIGHTPAREN: return three::KeyboardKey::right_paren;
        case SDLK_ASTERISK: return three::KeyboardKey::asterix;
        case SDLK_PLUS: return three::KeyboardKey::plus;
        case SDLK_COMMA: return three::KeyboardKey::comma;
        case SDLK_MINUS: return three::KeyboardKey::minus;
        case SDLK_PERIOD: return three::KeyboardKey::period;
        case SDLK_SLASH: return three::KeyboardKey::slash;
        case SDLK_0: return three::KeyboardKey::num_0;
        case SDLK_1: return three::KeyboardKey::num_1;
        case SDLK_2: return three::KeyboardKey::num_2;
        case SDLK_3: return three::KeyboardKey::num_3;
        case SDLK_4: return three::KeyboardKey::num_4;
        case SDLK_5: return three::KeyboardKey::num_5;
        case SDLK_6: return three::KeyboardKey::num_6;
        case SDLK_7: return three::KeyboardKey::num_7;
        case SDLK_8: return three::KeyboardKey::num_8;
        case SDLK_9: return three::KeyboardKey::num_9;
        case SDLK_COLON: return three::KeyboardKey::colon;
        case SDLK_SEMICOLON: return three::KeyboardKey::semicolon;
        case SDLK_LESS: return three::KeyboardKey::less;
        case SDLK_EQUALS: return three::KeyboardKey::equals;
        case SDLK_GREATER: return three::KeyboardKey::greater;
        case SDLK_QUESTION: return three::KeyboardKey::question;
        case SDLK_AT: return three::KeyboardKey::at;
        case SDLK_LEFTBRACKET: return three::KeyboardKey::left_bracket;
        case SDLK_BACKSLASH: return three::KeyboardKey::backslash;
        case SDLK_RIGHTBRACKET: return three::KeyboardKey::right_bracket;
        case SDLK_CARET: return three::KeyboardKey::caret;
        case SDLK_UNDERSCORE: return three::KeyboardKey::underscore;
        case SDLK_BACKQUOTE: return three::KeyboardKey::backquote;
        case SDLK_a: return three::KeyboardKey::a;
        case SDLK_b: return three::KeyboardKey::b;
        case SDLK_c: return three::KeyboardKey::c;
        case SDLK_d: return three::KeyboardKey::d;
        case SDLK_e: return three::KeyboardKey::e;
        case SDLK_f: return three::KeyboardKey::f;
        case SDLK_g: return three::KeyboardKey::g;
        case SDLK_h: return three::KeyboardKey::h;
        case SDLK_i: return three::KeyboardKey::i;
        case SDLK_j: return three::KeyboardKey::j;
        case SDLK_k: return three::KeyboardKey::k;
        case SDLK_l: return three::KeyboardKey::l;
        case SDLK_m: return three::KeyboardKey::m;
        case SDLK_n: return three::KeyboardKey::n;
        case SDLK_o: return three::KeyboardKey::o;
        case SDLK_p: return three::KeyboardKey::p;
        case SDLK_q: return three::KeyboardKey::q;
        case SDLK_r: return three::KeyboardKey::r;
        case SDLK_s: return three::KeyboardKey::s;
        case SDLK_t: return three::KeyboardKey::t;
        case SDLK_u: return three::KeyboardKey::u;
        case SDLK_v: return three::KeyboardKey::v;
        case SDLK_w: return three::KeyboardKey::w;
        case SDLK_x: return three::KeyboardKey::x;
        case SDLK_y: return three::KeyboardKey::y;
        case SDLK_z: return three::KeyboardKey::z;
        case SDLK_CAPSLOCK: return three::KeyboardKey::capslock;
        case SDLK_F1: return three::KeyboardKey::f1;
        case SDLK_F2: return three::KeyboardKey::f2;
        case SDLK_F3: return three::KeyboardKey::f3;
        case SDLK_F4: return three::KeyboardKey::f4;
        case SDLK_F5: return three::KeyboardKey::f5;
        case SDLK_F6: return three::KeyboardKey::f6;
        case SDLK_F7: return three::KeyboardKey::f7;
        case SDLK_F8: return three::KeyboardKey::f8;
        case SDLK_F9: return three::KeyboardKey::f9;
        case SDLK_F10: return three::KeyboardKey::f10;
        case SDLK_F11: return three::KeyboardKey::f11;
        case SDLK_F12: return three::KeyboardKey::f12;
        case SDLK_PRINTSCREEN: return three::KeyboardKey::printscreen;
        case SDLK_SCROLLLOCK: return three::KeyboardKey::scrolllock;
        case SDLK_PAUSE: return three::KeyboardKey::pause;
        case SDLK_INSERT: return three::KeyboardKey::insert;
        case SDLK_HOME: return three::KeyboardKey::home;
        case SDLK_PAGEUP: return three::KeyboardKey::pageup;
        case SDLK_DELETE: return three::KeyboardKey::delete_key;
        case SDLK_END: return three::KeyboardKey::end;
        case SDLK_PAGEDOWN: return three::KeyboardKey::pagedown;
        case SDLK_RIGHT: return three::KeyboardKey::right;
        case SDLK_LEFT: return three::KeyboardKey::left;
        case SDLK_DOWN: return three::KeyboardKey::down;
        case SDLK_UP: return three::KeyboardKey::up;
        case SDLK_NUMLOCKCLEAR: return three::KeyboardKey::numlock_clear;
        case SDLK_KP_DIVIDE: return three::KeyboardKey::keypad_divide;
        case SDLK_KP_MULTIPLY: return three::KeyboardKey::keypad_multiply;
        case SDLK_KP_MINUS: return three::KeyboardKey::keypad_minus;
        case SDLK_KP_PLUS: return three::KeyboardKey::keypad_plus;
        case SDLK_KP_ENTER: return three::KeyboardKey::keypad_enter;
        case SDLK_KP_1: return three::KeyboardKey::keypad_1;
        case SDLK_KP_2: return three::KeyboardKey::keypad_2;
        case SDLK_KP_3: return three::KeyboardKey::keypad_3;
        case SDLK_KP_4: return three::KeyboardKey::keypad_4;
        case SDLK_KP_5: return three::KeyboardKey::keypad_5;
        case SDLK_KP_6: return three::KeyboardKey::keypad_6;
        case SDLK_KP_7: return three::KeyboardKey::keypad_7;
        case SDLK_KP_8: return three::KeyboardKey::keypad_8;
        case SDLK_KP_9: return three::KeyboardKey::keypad_9;
        case SDLK_KP_0: return three::KeyboardKey::keypad_0;
        case SDLK_KP_PERIOD: return three::KeyboardKey::keypad_period;
        case SDLK_APPLICATION: return three::KeyboardKey::application;
        case SDLK_POWER: return three::KeyboardKey::power;
        case SDLK_KP_EQUALS: return three::KeyboardKey::keypad_equals;
        case SDLK_F13: return three::KeyboardKey::f13;
        case SDLK_F14: return three::KeyboardKey::f14;
        case SDLK_F15: return three::KeyboardKey::f15;
        case SDLK_F16: return three::KeyboardKey::f16;
        case SDLK_F17: return three::KeyboardKey::f17;
        case SDLK_F18: return three::KeyboardKey::f18;
        case SDLK_F19: return three::KeyboardKey::f19;
        case SDLK_F20: return three::KeyboardKey::f20;
        case SDLK_F21: return three::KeyboardKey::f21;
        case SDLK_F22: return three::KeyboardKey::f22;
        case SDLK_F23: return three::KeyboardKey::f23;
        case SDLK_F24: return three::KeyboardKey::f24;
        case SDLK_EXECUTE: return three::KeyboardKey::execute;
        case SDLK_HELP: return three::KeyboardKey::help;
        case SDLK_MENU: return three::KeyboardKey::menu;
        case SDLK_SELECT: return three::KeyboardKey::select;
        case SDLK_STOP: return three::KeyboardKey::stop;
        case SDLK_AGAIN: return three::KeyboardKey::again;
        case SDLK_UNDO: return three::KeyboardKey::undo;
        case SDLK_CUT: return three::KeyboardKey::cut;
        case SDLK_COPY: return three::KeyboardKey::copy;
        case SDLK_PASTE: return three::KeyboardKey::paste;
        case SDLK_FIND: return three::KeyboardKey::find;
        case SDLK_MUTE: return three::KeyboardKey::mute;
        case SDLK_VOLUMEUP: return three::KeyboardKey::volume_up;
        case SDLK_VOLUMEDOWN: return three::KeyboardKey::volume_down;
        case SDLK_KP_COMMA: return three::KeyboardKey::keypad_comma;
        case SDLK_KP_EQUALSAS400: return three::KeyboardKey::keypad_equals_as_400;
        case SDLK_ALTERASE: return three::KeyboardKey::alt_erase;
        case SDLK_SYSREQ: return three::KeyboardKey::sysreq;
        case SDLK_CANCEL: return three::KeyboardKey::cancel;
        case SDLK_CLEAR: return three::KeyboardKey::clear;
        case SDLK_PRIOR: return three::KeyboardKey::prior;
        case SDLK_RETURN2: return three::KeyboardKey::second_return;
        case SDLK_SEPARATOR: return three::KeyboardKey::separator;
        case SDLK_OUT: return three::KeyboardKey::out;
        case SDLK_OPER: return three::KeyboardKey::oper;
        case SDLK_CLEARAGAIN: return three::KeyboardKey::clear_again;
        case SDLK_CRSEL: return three::KeyboardKey::crsel;
        case SDLK_EXSEL: return three::KeyboardKey::exsel;
        case SDLK_KP_00: return three::KeyboardKey::keypad_00;
        case SDLK_KP_000: return three::KeyboardKey::keypad_000;
        case SDLK_THOUSANDSSEPARATOR: return three::KeyboardKey::thousand_separator;
        case SDLK_DECIMALSEPARATOR: return three::KeyboardKey::decimal_separator;
        case SDLK_CURRENCYUNIT: return three::KeyboardKey::currency_unit;
        case SDLK_CURRENCYSUBUNIT: return three::KeyboardKey::currency_subunit;
        case SDLK_KP_LEFTPAREN: return three::KeyboardKey::keypad_leftparen;
        case SDLK_KP_RIGHTPAREN: return three::KeyboardKey::keypad_rightparen;
        case SDLK_KP_LEFTBRACE: return three::KeyboardKey::keypad_leftbrace;
        case SDLK_KP_RIGHTBRACE: return three::KeyboardKey::keypad_rightbrace;
        case SDLK_KP_TAB: return three::KeyboardKey::keypad_tab;
        case SDLK_KP_BACKSPACE: return three::KeyboardKey::keypad_backspace;
        case SDLK_KP_A: return three::KeyboardKey::keypad_a;
        case SDLK_KP_B: return three::KeyboardKey::keypad_b;
        case SDLK_KP_C: return three::KeyboardKey::keypad_c;
        case SDLK_KP_D: return three::KeyboardKey::keypad_d;
        case SDLK_KP_E: return three::KeyboardKey::keypad_e;
        case SDLK_KP_F: return three::KeyboardKey::keypad_f;
        case SDLK_KP_XOR: return three::KeyboardKey::keypad_xor;
        case SDLK_KP_POWER: return three::KeyboardKey::keypad_power;
        case SDLK_KP_PERCENT: return three::KeyboardKey::keypad_percent;
        case SDLK_KP_LESS: return three::KeyboardKey::keypad_less;
        case SDLK_KP_GREATER: return three::KeyboardKey::keypad_greater;
        case SDLK_KP_AMPERSAND: return three::KeyboardKey::keypad_ampersand;
        case SDLK_KP_DBLAMPERSAND: return three::KeyboardKey::keypad_double_ampersand;
        case SDLK_KP_VERTICALBAR: return three::KeyboardKey::keypad_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR: return three::KeyboardKey::keypad_double_verticle_bar;
        case SDLK_KP_COLON: return three::KeyboardKey::keypad_colon;
        case SDLK_KP_HASH: return three::KeyboardKey::keypad_hash;
        case SDLK_KP_SPACE: return three::KeyboardKey::keypad_space;
        case SDLK_KP_AT: return three::KeyboardKey::keypad_at;
        case SDLK_KP_EXCLAM: return three::KeyboardKey::keypad_exclam;
        case SDLK_KP_MEMSTORE: return three::KeyboardKey::keypad_mem_store;
        case SDLK_KP_MEMRECALL: return three::KeyboardKey::keypad_mem_recall;
        case SDLK_KP_MEMCLEAR: return three::KeyboardKey::keypad_mem_clear;
        case SDLK_KP_MEMADD: return three::KeyboardKey::keypad_mem_add;
        case SDLK_KP_MEMSUBTRACT: return three::KeyboardKey::keypad_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return three::KeyboardKey::keypad_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return three::KeyboardKey::keypad_mem_divide;
        case SDLK_KP_PLUSMINUS: return three::KeyboardKey::keypad_plus_minus;
        case SDLK_KP_CLEAR: return three::KeyboardKey::keypad_clear;
        case SDLK_KP_CLEARENTRY: return three::KeyboardKey::keypad_clear_entry;
        case SDLK_KP_BINARY: return three::KeyboardKey::keypad_binary;
        case SDLK_KP_OCTAL: return three::KeyboardKey::keypad_octal;
        case SDLK_KP_DECIMAL: return three::KeyboardKey::keypad_decimal;
        case SDLK_KP_HEXADECIMAL: return three::KeyboardKey::keypad_hexadecimal;
        case SDLK_LCTRL: return three::KeyboardKey::ctrl_left;
        case SDLK_LSHIFT: return three::KeyboardKey::shift_left;
        case SDLK_LALT: return three::KeyboardKey::alt_left;
        case SDLK_LGUI: return three::KeyboardKey::gui_left;
        case SDLK_RCTRL: return three::KeyboardKey::ctrl_right;
        case SDLK_RSHIFT: return three::KeyboardKey::shift_right;
        case SDLK_RALT: return three::KeyboardKey::alt_right;
        case SDLK_RGUI: return three::KeyboardKey::gui_right;
        case SDLK_MODE: return three::KeyboardKey::mode;
        case SDLK_AUDIONEXT: return three::KeyboardKey::audio_next;
        case SDLK_AUDIOPREV: return three::KeyboardKey::audio_prev;
        case SDLK_AUDIOSTOP: return three::KeyboardKey::audio_stop;
        case SDLK_AUDIOPLAY: return three::KeyboardKey::audio_play;
        case SDLK_AUDIOMUTE: return three::KeyboardKey::audio_mute;
        case SDLK_MEDIASELECT: return three::KeyboardKey::mediaselect;
        case SDLK_WWW: return three::KeyboardKey::www;
        case SDLK_MAIL: return three::KeyboardKey::mail;
        case SDLK_CALCULATOR: return three::KeyboardKey::calculator;
        case SDLK_COMPUTER: return three::KeyboardKey::computer;
        case SDLK_AC_SEARCH: return three::KeyboardKey::ac_search;
        case SDLK_AC_HOME: return three::KeyboardKey::ac_home;
        case SDLK_AC_BACK: return three::KeyboardKey::ac_back;
        case SDLK_AC_FORWARD: return three::KeyboardKey::ac_forward;
        case SDLK_AC_STOP: return three::KeyboardKey::ac_stop;
        case SDLK_AC_REFRESH: return three::KeyboardKey::ac_refresh;
        case SDLK_AC_BOOKMARKS: return three::KeyboardKey::ac_bookmarks;
        case SDLK_BRIGHTNESSDOWN: return three::KeyboardKey::brightness_down;
        case SDLK_BRIGHTNESSUP: return three::KeyboardKey::brightness_up;
        case SDLK_DISPLAYSWITCH: return three::KeyboardKey::displayswitch;
        case SDLK_KBDILLUMTOGGLE: return three::KeyboardKey::keyboard_illum_toggle;
        case SDLK_KBDILLUMDOWN: return three::KeyboardKey::keyboard_illum_down;
        case SDLK_KBDILLUMUP: return three::KeyboardKey::keyboard_illum_up;
        case SDLK_EJECT: return three::KeyboardKey::eject;
        case SDLK_SLEEP: return three::KeyboardKey::sleep;
        default:
            DIE("Invalid keyboard button");
            return three::KeyboardKey::invalid;
    }
}


three::MouseButton to_mouse_button(Uint8 mb)
{
    switch (mb)
    {
        case SDL_BUTTON_LEFT: return three::MouseButton::left;
        case SDL_BUTTON_MIDDLE: return three::MouseButton::middle;
        case SDL_BUTTON_RIGHT: return three::MouseButton::right;
        case SDL_BUTTON_X1: return three::MouseButton::x1;
        case SDL_BUTTON_X2: return three::MouseButton::x2;
        default:
            DIE("Invalid mouse button");
            return three::MouseButton::invalid;
    }
}


three::GamecontrollerButton to_controller_button(SDL_GameControllerButton b)
{
    switch(b)
    {
        case SDL_CONTROLLER_BUTTON_A: return three::GamecontrollerButton::a;
        case SDL_CONTROLLER_BUTTON_B: return three::GamecontrollerButton::b;
        case SDL_CONTROLLER_BUTTON_X: return three::GamecontrollerButton::x;
        case SDL_CONTROLLER_BUTTON_Y: return three::GamecontrollerButton::y;
        case SDL_CONTROLLER_BUTTON_BACK: return three::GamecontrollerButton::back;
        case SDL_CONTROLLER_BUTTON_GUIDE: return three::GamecontrollerButton::guide;
        case SDL_CONTROLLER_BUTTON_START: return three::GamecontrollerButton::start;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return three::GamecontrollerButton::stick_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return three::GamecontrollerButton::stick_right;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return three::GamecontrollerButton::shoulder_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return three::GamecontrollerButton::shoulder_right;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return three::GamecontrollerButton::dpad_up;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return three::GamecontrollerButton::dpad_down;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return three::GamecontrollerButton::dpad_left;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return three::GamecontrollerButton::dpad_right;
#if SDL_VERSION_ATLEAST(2, 0, 14)
        case SDL_CONTROLLER_BUTTON_MISC1: return three::GamecontrollerButton::misc1;
        case SDL_CONTROLLER_BUTTON_PADDLE1: return three::GamecontrollerButton::paddle1;
        case SDL_CONTROLLER_BUTTON_PADDLE2: return three::GamecontrollerButton::paddle2;
        case SDL_CONTROLLER_BUTTON_PADDLE3: return three::GamecontrollerButton::paddle3;
        case SDL_CONTROLLER_BUTTON_PADDLE4: return three::GamecontrollerButton::paddle4;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD: return three::GamecontrollerButton::touchpad;
#endif
        default:
        return three::GamecontrollerButton::invalid;
    }
}

three::GamecontrollerButton to_controller_button(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return three::GamecontrollerButton::trigger_left;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return three::GamecontrollerButton::trigger_right;
        default:
        return three::GamecontrollerButton::invalid;
    }
}

three::GamecontrollerAxis to_controller_axis(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_LEFTX: return three::GamecontrollerAxis::left_x;
        case SDL_CONTROLLER_AXIS_LEFTY: return three::GamecontrollerAxis::left_y;
        case SDL_CONTROLLER_AXIS_RIGHTX: return three::GamecontrollerAxis::right_x;
        case SDL_CONTROLLER_AXIS_RIGHTY: return three::GamecontrollerAxis::right_y;
        default: return three::GamecontrollerAxis::invalid;
    }
}


}
