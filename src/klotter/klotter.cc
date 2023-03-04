#include "klotter/klotter.h"
#include "klotter/dependency_glad.h"
#include "klotter/render/opengl_utils.h"


namespace klotter
{

int run_main(MakeAppFunction make_app)
{
    constexpr int start_width = 800;
    constexpr int starth_height = 600;

    constexpr Uint32 flags = SDL_INIT_VIDEO;
    if(SDL_Init(flags) != 0)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    auto* sdl_window = SDL_CreateWindow
    (
        "klotter",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        start_width,
        starth_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if(sdl_window == nullptr)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
        return -1;
    }

    auto sdl_glcontext = SDL_GL_CreateContext(sdl_window);

    if(sdl_glcontext == nullptr)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create gl context: %s", SDL_GetError());

        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;

        return -1;
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to load OpenGL");

        SDL_GL_DeleteContext(sdl_glcontext);
        sdl_glcontext = nullptr;

        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;

        return -1;
    }

    setup_opengl_debug();

    bool running = true;
    auto app = make_app();

    int window_width = start_width;
    int window_height = starth_height;

    int frame_skip = 3; // wait a few frames so the fps can stabilize

    bool mouse = false;
    bool w = false;
    bool a = false;
    bool s = false;
    bool d = false;
    bool space = false;
    bool lctrl = false;

    auto last = SDL_GetPerformanceCounter();
    while (running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto diff = static_cast<float>(now - last);
        const auto freq = static_cast<float>(SDL_GetPerformanceFrequency());
        const auto dt = frame_skip > 0 ? 0.0f : diff / freq;
        last = now;

        if (frame_skip > 0) { frame_skip -= 1; }

        // handle events
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_WINDOWEVENT:
                switch (e.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    window_width = e.window.data1;
                    window_height = e.window.data2;
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;
                    break;
                }
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                const auto down = e.type == SDL_KEYDOWN;
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_W: w = down; break;
                case SDL_SCANCODE_A: a = down; break;
                case SDL_SCANCODE_S: s = down; break;
                case SDL_SCANCODE_D: d = down; break;
                case SDL_SCANCODE_SPACE: space = down; break;
                case SDL_SCANCODE_LCTRL: lctrl = down; break;
                default:
                    // ignore other keys
                    break;
                }
            }
            break;

            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            {
                const auto down = e.type == SDL_MOUSEBUTTONDOWN;
                if (e.button.button == 1)
                {
                    mouse = down;
                    SDL_SetRelativeMouseMode(mouse ? SDL_TRUE : SDL_FALSE);
                }
            }
            break;

            case SDL_MOUSEMOTION:
                if (mouse)
                {
                    const float sensitivity = 0.25f;
                    app->camera.yaw += static_cast<float>(e.motion.xrel) * sensitivity;
                    app->camera.pitch -= static_cast<float>(e.motion.yrel) * sensitivity;

                    if (app->camera.pitch > 89.0f) { app->camera.pitch = 89.0f; }
                    if (app->camera.pitch < -89.0f) { app->camera.pitch = -89.0f; }
                }
                break;

            case SDL_QUIT:
                running = false;
                break;
            default:
                break;
            }
        }

        // update
        if(mouse)
        {
            auto& cam = app->camera;
            const auto v = create_vectors(cam);
            
            const auto move = [](bool p, bool n) -> std::optional<float>
                { if (p != n) { return p ? 1.0f : -1.0f; }
                else { return std::nullopt; } };

            if (const auto m = move(w, s); m) { cam.position += v.front * *m * dt; }
            if (const auto m = move(d, a); m) { cam.position += v.right * *m * dt; }
            if (const auto m = move(space, lctrl); m) { cam.position += v.up * *m * dt; }

        }
        // render
        app->renderer.window_size = {window_width, window_height};
        app->on_render(dt);

        SDL_GL_SwapWindow(sdl_window);
    }

    SDL_GL_DeleteContext(sdl_glcontext);
    sdl_glcontext = nullptr;
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
    return 0;
}


}

