#include "klotter/klotter.h"
#include "klotter/dependency_glad.h"
#include "klotter/render/opengl_utils.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace klotter
{

int run_main(MakeAppFunction make_app)
{
    constexpr int start_width = 800;
    constexpr int starth_height = 600;




    ////////////////////////////////////////////////////////////////////////////////
    // sdl config

    constexpr Uint32 flags = SDL_INIT_VIDEO;
    if(SDL_Init(flags) != 0)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); // was 0 in dear imgui example??
#endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);


    ////////////////////////////////////////////////////////////////////////////////
    // create window

    auto* sdl_window = SDL_CreateWindow
    (
        "klotter",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        start_width,
        starth_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
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

    SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to load OpenGL");

        SDL_GL_DeleteContext(sdl_glcontext);
        sdl_glcontext = nullptr;

        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // dear imgui setup

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForOpenGL(sdl_window, sdl_glcontext);
    ImGui_ImplOpenGL3_Init(glsl_version);


    ///////////////////////////////////////////////////////////////
    // complete setup
    setup_opengl_debug();



    ////////////////////////////////////////////////////////////////
    // run app
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
        auto scene = app->scene;

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
            ImGui_ImplSDL2_ProcessEvent(&e);

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
                if(ImGui::GetIO().WantCaptureMouse == false)
                {
                    const auto down = e.type == SDL_MOUSEBUTTONDOWN;
                    if (e.button.button == 1)
                    {
                        mouse = down;
                        SDL_SetRelativeMouseMode(mouse ? SDL_TRUE : SDL_FALSE);
                    }
                }
            }
            break;

            case SDL_MOUSEMOTION:
                if (mouse)
                {
                    const float sensitivity = 0.25f;
                    scene->camera.yaw += static_cast<float>(e.motion.xrel) * sensitivity;
                    scene->camera.pitch -= static_cast<float>(e.motion.yrel) * sensitivity;

                    if (scene->camera.pitch > 89.0f) { scene->camera.pitch = 89.0f; }
                    if (scene->camera.pitch < -89.0f) { scene->camera.pitch = -89.0f; }
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
            auto& cam = scene->camera;
            const auto v = create_vectors(cam);
            
            const auto move = [](bool p, bool n) -> std::optional<float>
                { if (p != n) { return p ? 1.0f : -1.0f; }
                else { return std::nullopt; } };

            if (const auto m = move(w, s); m) { cam.position += v.front * *m * dt; }
            if (const auto m = move(d, a); m) { cam.position += v.right * *m * dt; }
            if (const auto m = move(space, lctrl); m) { cam.position += v.up * *m * dt; }

        }


        // imgui windows
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        app->on_gui();
        ImGui::Render();


        // render
        app->renderer.window_size = {window_width, window_height};
        scene->on_render(dt);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(sdl_window);
    }



    /////////////////////////////////////////////////////////////////////////////////////////
    // cleanup

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(sdl_glcontext);
    sdl_glcontext = nullptr;
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
    return 0;
}


}

