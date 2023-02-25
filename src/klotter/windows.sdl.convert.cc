#include "klotter/windows.sdl.convert.h"

#include "klotter/assert.h"


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


klotter::KeyboardKey to_keyboard_key(SDL_Keysym key)
{
    switch (key.sym)
    {
        case SDLK_RETURN: return klotter::KeyboardKey::return_key;
        case SDLK_ESCAPE: return klotter::KeyboardKey::escape;
        case SDLK_BACKSPACE: return klotter::KeyboardKey::backspace;
        case SDLK_TAB: return klotter::KeyboardKey::tab;
        case SDLK_SPACE: return klotter::KeyboardKey::space;
        case SDLK_EXCLAIM: return klotter::KeyboardKey::exclaim;
        case SDLK_QUOTEDBL: return klotter::KeyboardKey::quotedbl;
        case SDLK_HASH: return klotter::KeyboardKey::hash;
        case SDLK_PERCENT: return klotter::KeyboardKey::percent;
        case SDLK_DOLLAR: return klotter::KeyboardKey::dollar;
        case SDLK_AMPERSAND: return klotter::KeyboardKey::ampersand;
        case SDLK_QUOTE: return klotter::KeyboardKey::quote;
        case SDLK_LEFTPAREN: return klotter::KeyboardKey::left_paren;
        case SDLK_RIGHTPAREN: return klotter::KeyboardKey::right_paren;
        case SDLK_ASTERISK: return klotter::KeyboardKey::asterix;
        case SDLK_PLUS: return klotter::KeyboardKey::plus;
        case SDLK_COMMA: return klotter::KeyboardKey::comma;
        case SDLK_MINUS: return klotter::KeyboardKey::minus;
        case SDLK_PERIOD: return klotter::KeyboardKey::period;
        case SDLK_SLASH: return klotter::KeyboardKey::slash;
        case SDLK_0: return klotter::KeyboardKey::num_0;
        case SDLK_1: return klotter::KeyboardKey::num_1;
        case SDLK_2: return klotter::KeyboardKey::num_2;
        case SDLK_3: return klotter::KeyboardKey::num_3;
        case SDLK_4: return klotter::KeyboardKey::num_4;
        case SDLK_5: return klotter::KeyboardKey::num_5;
        case SDLK_6: return klotter::KeyboardKey::num_6;
        case SDLK_7: return klotter::KeyboardKey::num_7;
        case SDLK_8: return klotter::KeyboardKey::num_8;
        case SDLK_9: return klotter::KeyboardKey::num_9;
        case SDLK_COLON: return klotter::KeyboardKey::colon;
        case SDLK_SEMICOLON: return klotter::KeyboardKey::semicolon;
        case SDLK_LESS: return klotter::KeyboardKey::less;
        case SDLK_EQUALS: return klotter::KeyboardKey::equals;
        case SDLK_GREATER: return klotter::KeyboardKey::greater;
        case SDLK_QUESTION: return klotter::KeyboardKey::question;
        case SDLK_AT: return klotter::KeyboardKey::at;
        case SDLK_LEFTBRACKET: return klotter::KeyboardKey::left_bracket;
        case SDLK_BACKSLASH: return klotter::KeyboardKey::backslash;
        case SDLK_RIGHTBRACKET: return klotter::KeyboardKey::right_bracket;
        case SDLK_CARET: return klotter::KeyboardKey::caret;
        case SDLK_UNDERSCORE: return klotter::KeyboardKey::underscore;
        case SDLK_BACKQUOTE: return klotter::KeyboardKey::backquote;
        case SDLK_a: return klotter::KeyboardKey::a;
        case SDLK_b: return klotter::KeyboardKey::b;
        case SDLK_c: return klotter::KeyboardKey::c;
        case SDLK_d: return klotter::KeyboardKey::d;
        case SDLK_e: return klotter::KeyboardKey::e;
        case SDLK_f: return klotter::KeyboardKey::f;
        case SDLK_g: return klotter::KeyboardKey::g;
        case SDLK_h: return klotter::KeyboardKey::h;
        case SDLK_i: return klotter::KeyboardKey::i;
        case SDLK_j: return klotter::KeyboardKey::j;
        case SDLK_k: return klotter::KeyboardKey::k;
        case SDLK_l: return klotter::KeyboardKey::l;
        case SDLK_m: return klotter::KeyboardKey::m;
        case SDLK_n: return klotter::KeyboardKey::n;
        case SDLK_o: return klotter::KeyboardKey::o;
        case SDLK_p: return klotter::KeyboardKey::p;
        case SDLK_q: return klotter::KeyboardKey::q;
        case SDLK_r: return klotter::KeyboardKey::r;
        case SDLK_s: return klotter::KeyboardKey::s;
        case SDLK_t: return klotter::KeyboardKey::t;
        case SDLK_u: return klotter::KeyboardKey::u;
        case SDLK_v: return klotter::KeyboardKey::v;
        case SDLK_w: return klotter::KeyboardKey::w;
        case SDLK_x: return klotter::KeyboardKey::x;
        case SDLK_y: return klotter::KeyboardKey::y;
        case SDLK_z: return klotter::KeyboardKey::z;
        case SDLK_CAPSLOCK: return klotter::KeyboardKey::capslock;
        case SDLK_F1: return klotter::KeyboardKey::f1;
        case SDLK_F2: return klotter::KeyboardKey::f2;
        case SDLK_F3: return klotter::KeyboardKey::f3;
        case SDLK_F4: return klotter::KeyboardKey::f4;
        case SDLK_F5: return klotter::KeyboardKey::f5;
        case SDLK_F6: return klotter::KeyboardKey::f6;
        case SDLK_F7: return klotter::KeyboardKey::f7;
        case SDLK_F8: return klotter::KeyboardKey::f8;
        case SDLK_F9: return klotter::KeyboardKey::f9;
        case SDLK_F10: return klotter::KeyboardKey::f10;
        case SDLK_F11: return klotter::KeyboardKey::f11;
        case SDLK_F12: return klotter::KeyboardKey::f12;
        case SDLK_PRINTSCREEN: return klotter::KeyboardKey::printscreen;
        case SDLK_SCROLLLOCK: return klotter::KeyboardKey::scrolllock;
        case SDLK_PAUSE: return klotter::KeyboardKey::pause;
        case SDLK_INSERT: return klotter::KeyboardKey::insert;
        case SDLK_HOME: return klotter::KeyboardKey::home;
        case SDLK_PAGEUP: return klotter::KeyboardKey::pageup;
        case SDLK_DELETE: return klotter::KeyboardKey::delete_key;
        case SDLK_END: return klotter::KeyboardKey::end;
        case SDLK_PAGEDOWN: return klotter::KeyboardKey::pagedown;
        case SDLK_RIGHT: return klotter::KeyboardKey::right;
        case SDLK_LEFT: return klotter::KeyboardKey::left;
        case SDLK_DOWN: return klotter::KeyboardKey::down;
        case SDLK_UP: return klotter::KeyboardKey::up;
        case SDLK_NUMLOCKCLEAR: return klotter::KeyboardKey::numlock_clear;
        case SDLK_KP_DIVIDE: return klotter::KeyboardKey::keypad_divide;
        case SDLK_KP_MULTIPLY: return klotter::KeyboardKey::keypad_multiply;
        case SDLK_KP_MINUS: return klotter::KeyboardKey::keypad_minus;
        case SDLK_KP_PLUS: return klotter::KeyboardKey::keypad_plus;
        case SDLK_KP_ENTER: return klotter::KeyboardKey::keypad_enter;
        case SDLK_KP_1: return klotter::KeyboardKey::keypad_1;
        case SDLK_KP_2: return klotter::KeyboardKey::keypad_2;
        case SDLK_KP_3: return klotter::KeyboardKey::keypad_3;
        case SDLK_KP_4: return klotter::KeyboardKey::keypad_4;
        case SDLK_KP_5: return klotter::KeyboardKey::keypad_5;
        case SDLK_KP_6: return klotter::KeyboardKey::keypad_6;
        case SDLK_KP_7: return klotter::KeyboardKey::keypad_7;
        case SDLK_KP_8: return klotter::KeyboardKey::keypad_8;
        case SDLK_KP_9: return klotter::KeyboardKey::keypad_9;
        case SDLK_KP_0: return klotter::KeyboardKey::keypad_0;
        case SDLK_KP_PERIOD: return klotter::KeyboardKey::keypad_period;
        case SDLK_APPLICATION: return klotter::KeyboardKey::application;
        case SDLK_POWER: return klotter::KeyboardKey::power;
        case SDLK_KP_EQUALS: return klotter::KeyboardKey::keypad_equals;
        case SDLK_F13: return klotter::KeyboardKey::f13;
        case SDLK_F14: return klotter::KeyboardKey::f14;
        case SDLK_F15: return klotter::KeyboardKey::f15;
        case SDLK_F16: return klotter::KeyboardKey::f16;
        case SDLK_F17: return klotter::KeyboardKey::f17;
        case SDLK_F18: return klotter::KeyboardKey::f18;
        case SDLK_F19: return klotter::KeyboardKey::f19;
        case SDLK_F20: return klotter::KeyboardKey::f20;
        case SDLK_F21: return klotter::KeyboardKey::f21;
        case SDLK_F22: return klotter::KeyboardKey::f22;
        case SDLK_F23: return klotter::KeyboardKey::f23;
        case SDLK_F24: return klotter::KeyboardKey::f24;
        case SDLK_EXECUTE: return klotter::KeyboardKey::execute;
        case SDLK_HELP: return klotter::KeyboardKey::help;
        case SDLK_MENU: return klotter::KeyboardKey::menu;
        case SDLK_SELECT: return klotter::KeyboardKey::select;
        case SDLK_STOP: return klotter::KeyboardKey::stop;
        case SDLK_AGAIN: return klotter::KeyboardKey::again;
        case SDLK_UNDO: return klotter::KeyboardKey::undo;
        case SDLK_CUT: return klotter::KeyboardKey::cut;
        case SDLK_COPY: return klotter::KeyboardKey::copy;
        case SDLK_PASTE: return klotter::KeyboardKey::paste;
        case SDLK_FIND: return klotter::KeyboardKey::find;
        case SDLK_MUTE: return klotter::KeyboardKey::mute;
        case SDLK_VOLUMEUP: return klotter::KeyboardKey::volume_up;
        case SDLK_VOLUMEDOWN: return klotter::KeyboardKey::volume_down;
        case SDLK_KP_COMMA: return klotter::KeyboardKey::keypad_comma;
        case SDLK_KP_EQUALSAS400: return klotter::KeyboardKey::keypad_equals_as_400;
        case SDLK_ALTERASE: return klotter::KeyboardKey::alt_erase;
        case SDLK_SYSREQ: return klotter::KeyboardKey::sysreq;
        case SDLK_CANCEL: return klotter::KeyboardKey::cancel;
        case SDLK_CLEAR: return klotter::KeyboardKey::clear;
        case SDLK_PRIOR: return klotter::KeyboardKey::prior;
        case SDLK_RETURN2: return klotter::KeyboardKey::second_return;
        case SDLK_SEPARATOR: return klotter::KeyboardKey::separator;
        case SDLK_OUT: return klotter::KeyboardKey::out;
        case SDLK_OPER: return klotter::KeyboardKey::oper;
        case SDLK_CLEARAGAIN: return klotter::KeyboardKey::clear_again;
        case SDLK_CRSEL: return klotter::KeyboardKey::crsel;
        case SDLK_EXSEL: return klotter::KeyboardKey::exsel;
        case SDLK_KP_00: return klotter::KeyboardKey::keypad_00;
        case SDLK_KP_000: return klotter::KeyboardKey::keypad_000;
        case SDLK_THOUSANDSSEPARATOR: return klotter::KeyboardKey::thousand_separator;
        case SDLK_DECIMALSEPARATOR: return klotter::KeyboardKey::decimal_separator;
        case SDLK_CURRENCYUNIT: return klotter::KeyboardKey::currency_unit;
        case SDLK_CURRENCYSUBUNIT: return klotter::KeyboardKey::currency_subunit;
        case SDLK_KP_LEFTPAREN: return klotter::KeyboardKey::keypad_leftparen;
        case SDLK_KP_RIGHTPAREN: return klotter::KeyboardKey::keypad_rightparen;
        case SDLK_KP_LEFTBRACE: return klotter::KeyboardKey::keypad_leftbrace;
        case SDLK_KP_RIGHTBRACE: return klotter::KeyboardKey::keypad_rightbrace;
        case SDLK_KP_TAB: return klotter::KeyboardKey::keypad_tab;
        case SDLK_KP_BACKSPACE: return klotter::KeyboardKey::keypad_backspace;
        case SDLK_KP_A: return klotter::KeyboardKey::keypad_a;
        case SDLK_KP_B: return klotter::KeyboardKey::keypad_b;
        case SDLK_KP_C: return klotter::KeyboardKey::keypad_c;
        case SDLK_KP_D: return klotter::KeyboardKey::keypad_d;
        case SDLK_KP_E: return klotter::KeyboardKey::keypad_e;
        case SDLK_KP_F: return klotter::KeyboardKey::keypad_f;
        case SDLK_KP_XOR: return klotter::KeyboardKey::keypad_xor;
        case SDLK_KP_POWER: return klotter::KeyboardKey::keypad_power;
        case SDLK_KP_PERCENT: return klotter::KeyboardKey::keypad_percent;
        case SDLK_KP_LESS: return klotter::KeyboardKey::keypad_less;
        case SDLK_KP_GREATER: return klotter::KeyboardKey::keypad_greater;
        case SDLK_KP_AMPERSAND: return klotter::KeyboardKey::keypad_ampersand;
        case SDLK_KP_DBLAMPERSAND: return klotter::KeyboardKey::keypad_double_ampersand;
        case SDLK_KP_VERTICALBAR: return klotter::KeyboardKey::keypad_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR: return klotter::KeyboardKey::keypad_double_verticle_bar;
        case SDLK_KP_COLON: return klotter::KeyboardKey::keypad_colon;
        case SDLK_KP_HASH: return klotter::KeyboardKey::keypad_hash;
        case SDLK_KP_SPACE: return klotter::KeyboardKey::keypad_space;
        case SDLK_KP_AT: return klotter::KeyboardKey::keypad_at;
        case SDLK_KP_EXCLAM: return klotter::KeyboardKey::keypad_exclam;
        case SDLK_KP_MEMSTORE: return klotter::KeyboardKey::keypad_mem_store;
        case SDLK_KP_MEMRECALL: return klotter::KeyboardKey::keypad_mem_recall;
        case SDLK_KP_MEMCLEAR: return klotter::KeyboardKey::keypad_mem_clear;
        case SDLK_KP_MEMADD: return klotter::KeyboardKey::keypad_mem_add;
        case SDLK_KP_MEMSUBTRACT: return klotter::KeyboardKey::keypad_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return klotter::KeyboardKey::keypad_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return klotter::KeyboardKey::keypad_mem_divide;
        case SDLK_KP_PLUSMINUS: return klotter::KeyboardKey::keypad_plus_minus;
        case SDLK_KP_CLEAR: return klotter::KeyboardKey::keypad_clear;
        case SDLK_KP_CLEARENTRY: return klotter::KeyboardKey::keypad_clear_entry;
        case SDLK_KP_BINARY: return klotter::KeyboardKey::keypad_binary;
        case SDLK_KP_OCTAL: return klotter::KeyboardKey::keypad_octal;
        case SDLK_KP_DECIMAL: return klotter::KeyboardKey::keypad_decimal;
        case SDLK_KP_HEXADECIMAL: return klotter::KeyboardKey::keypad_hexadecimal;
        case SDLK_LCTRL: return klotter::KeyboardKey::ctrl_left;
        case SDLK_LSHIFT: return klotter::KeyboardKey::shift_left;
        case SDLK_LALT: return klotter::KeyboardKey::alt_left;
        case SDLK_LGUI: return klotter::KeyboardKey::gui_left;
        case SDLK_RCTRL: return klotter::KeyboardKey::ctrl_right;
        case SDLK_RSHIFT: return klotter::KeyboardKey::shift_right;
        case SDLK_RALT: return klotter::KeyboardKey::alt_right;
        case SDLK_RGUI: return klotter::KeyboardKey::gui_right;
        case SDLK_MODE: return klotter::KeyboardKey::mode;
        case SDLK_AUDIONEXT: return klotter::KeyboardKey::audio_next;
        case SDLK_AUDIOPREV: return klotter::KeyboardKey::audio_prev;
        case SDLK_AUDIOSTOP: return klotter::KeyboardKey::audio_stop;
        case SDLK_AUDIOPLAY: return klotter::KeyboardKey::audio_play;
        case SDLK_AUDIOMUTE: return klotter::KeyboardKey::audio_mute;
        case SDLK_MEDIASELECT: return klotter::KeyboardKey::mediaselect;
        case SDLK_WWW: return klotter::KeyboardKey::www;
        case SDLK_MAIL: return klotter::KeyboardKey::mail;
        case SDLK_CALCULATOR: return klotter::KeyboardKey::calculator;
        case SDLK_COMPUTER: return klotter::KeyboardKey::computer;
        case SDLK_AC_SEARCH: return klotter::KeyboardKey::ac_search;
        case SDLK_AC_HOME: return klotter::KeyboardKey::ac_home;
        case SDLK_AC_BACK: return klotter::KeyboardKey::ac_back;
        case SDLK_AC_FORWARD: return klotter::KeyboardKey::ac_forward;
        case SDLK_AC_STOP: return klotter::KeyboardKey::ac_stop;
        case SDLK_AC_REFRESH: return klotter::KeyboardKey::ac_refresh;
        case SDLK_AC_BOOKMARKS: return klotter::KeyboardKey::ac_bookmarks;
        case SDLK_BRIGHTNESSDOWN: return klotter::KeyboardKey::brightness_down;
        case SDLK_BRIGHTNESSUP: return klotter::KeyboardKey::brightness_up;
        case SDLK_DISPLAYSWITCH: return klotter::KeyboardKey::displayswitch;
        case SDLK_KBDILLUMTOGGLE: return klotter::KeyboardKey::keyboard_illum_toggle;
        case SDLK_KBDILLUMDOWN: return klotter::KeyboardKey::keyboard_illum_down;
        case SDLK_KBDILLUMUP: return klotter::KeyboardKey::keyboard_illum_up;
        case SDLK_EJECT: return klotter::KeyboardKey::eject;
        case SDLK_SLEEP: return klotter::KeyboardKey::sleep;
        default:
            DIE("Invalid keyboard button");
            return klotter::KeyboardKey::invalid;
    }
}


klotter::MouseButton to_mouse_button(Uint8 mb)
{
    switch (mb)
    {
        case SDL_BUTTON_LEFT: return klotter::MouseButton::left;
        case SDL_BUTTON_MIDDLE: return klotter::MouseButton::middle;
        case SDL_BUTTON_RIGHT: return klotter::MouseButton::right;
        case SDL_BUTTON_X1: return klotter::MouseButton::x1;
        case SDL_BUTTON_X2: return klotter::MouseButton::x2;
        default:
            DIE("Invalid mouse button");
            return klotter::MouseButton::invalid;
    }
}


klotter::GamecontrollerButton to_controller_button(SDL_GameControllerButton b)
{
    switch(b)
    {
        case SDL_CONTROLLER_BUTTON_A: return klotter::GamecontrollerButton::a;
        case SDL_CONTROLLER_BUTTON_B: return klotter::GamecontrollerButton::b;
        case SDL_CONTROLLER_BUTTON_X: return klotter::GamecontrollerButton::x;
        case SDL_CONTROLLER_BUTTON_Y: return klotter::GamecontrollerButton::y;
        case SDL_CONTROLLER_BUTTON_BACK: return klotter::GamecontrollerButton::back;
        case SDL_CONTROLLER_BUTTON_GUIDE: return klotter::GamecontrollerButton::guide;
        case SDL_CONTROLLER_BUTTON_START: return klotter::GamecontrollerButton::start;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return klotter::GamecontrollerButton::stick_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return klotter::GamecontrollerButton::stick_right;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return klotter::GamecontrollerButton::shoulder_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return klotter::GamecontrollerButton::shoulder_right;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return klotter::GamecontrollerButton::dpad_up;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return klotter::GamecontrollerButton::dpad_down;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return klotter::GamecontrollerButton::dpad_left;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return klotter::GamecontrollerButton::dpad_right;
#if SDL_VERSION_ATLEAST(2, 0, 14)
        case SDL_CONTROLLER_BUTTON_MISC1: return klotter::GamecontrollerButton::misc1;
        case SDL_CONTROLLER_BUTTON_PADDLE1: return klotter::GamecontrollerButton::paddle1;
        case SDL_CONTROLLER_BUTTON_PADDLE2: return klotter::GamecontrollerButton::paddle2;
        case SDL_CONTROLLER_BUTTON_PADDLE3: return klotter::GamecontrollerButton::paddle3;
        case SDL_CONTROLLER_BUTTON_PADDLE4: return klotter::GamecontrollerButton::paddle4;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD: return klotter::GamecontrollerButton::touchpad;
#endif
        default:
        return klotter::GamecontrollerButton::invalid;
    }
}

klotter::GamecontrollerButton to_controller_button(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return klotter::GamecontrollerButton::trigger_left;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return klotter::GamecontrollerButton::trigger_right;
        default:
        return klotter::GamecontrollerButton::invalid;
    }
}

klotter::GamecontrollerAxis to_controller_axis(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_LEFTX: return klotter::GamecontrollerAxis::left_x;
        case SDL_CONTROLLER_AXIS_LEFTY: return klotter::GamecontrollerAxis::left_y;
        case SDL_CONTROLLER_AXIS_RIGHTX: return klotter::GamecontrollerAxis::right_x;
        case SDL_CONTROLLER_AXIS_RIGHTY: return klotter::GamecontrollerAxis::right_y;
        default: return klotter::GamecontrollerAxis::invalid;
    }
}


}
