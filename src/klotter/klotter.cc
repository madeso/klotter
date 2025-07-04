#include "klotter/klotter.h"
#include "klotter/dependency_glad.h"
#include "klotter/render/opengl_utils.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "klotter/log.h"

namespace klotter
{

constexpr int start_width = 800;
constexpr int start_height = 600;

int app_main(const RenderSettings& rs, MakeAppFunction make_app, SDL_Window* sdl_window)
{
	////////////////////////////////////////////////////////////////
	// create actual app
	Renderer renderer{rs};
	if (renderer.is_loaded() == false)
	{
		return -1;
	}

	auto app = make_app(&renderer);


	////////////////////////////////////////////////////////////////
	// run app
	bool running = true;

	int window_width = start_width;
	int window_height = start_height;

	SDL_GetWindowSize(sdl_window, &window_width, &window_height);

	int frame_skip = 2;	 // wait a few frames so the fps can stabilize

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
		app->on_frame(&renderer);
		const auto now = SDL_GetPerformanceCounter();
		const auto diff = static_cast<float>(now - last);
		const auto freq = static_cast<float>(SDL_GetPerformanceFrequency());
		const auto dt = frame_skip > 0 ? 0.0f : diff / freq;
		last = now;

		if (frame_skip > 0)
		{
			frame_skip -= 1;
		}

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
				case SDL_WINDOWEVENT_CLOSE: running = false; break;
				default:
					// ignore other events
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
					if (ImGui::GetIO().WantCaptureMouse == false)
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
					app->camera.yaw += static_cast<float>(e.motion.xrel) * sensitivity;
					app->camera.pitch -= static_cast<float>(e.motion.yrel) * sensitivity;

					if (app->camera.pitch > 89.0f)
					{
						app->camera.pitch = 89.0f;
					}
					if (app->camera.pitch < -89.0f)
					{
						app->camera.pitch = -89.0f;
					}
				}
				break;

			case SDL_QUIT: running = false; break;
			default: break;
			}
		}

		// update
		if (mouse)
		{
			auto& cam = app->camera;
			const auto v = create_vectors(cam);

			const auto move = [](bool p, bool n) -> std::optional<float>
			{
				if (p != n)
				{
					return p ? 1.0f : -1.0f;
				}
				else
				{
					return std::nullopt;
				}
			};

			if (const auto m = move(w, s); m)
			{
				cam.position += v.front * *m * dt;
			}
			if (const auto m = move(d, a); m)
			{
				cam.position += v.right * *m * dt;
			}
			if (const auto m = move(space, lctrl); m)
			{
				cam.position += v.up * *m * dt;
			}
		}


		// imgui windows
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		app->on_gui(&renderer);

		ImGui::Render();


		// render
		{
			SCOPED_DEBUG_GROUP("Render App"sv);
			app->on_render({window_width, window_height}, &renderer, dt);
		}

		{
			SCOPED_DEBUG_GROUP("DearImGui rendering"sv);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		SDL_GL_SwapWindow(sdl_window);
	}

	return 0;
}

int run_main(const RenderSettings& rs, MakeAppFunction make_app)
{
	////////////////////////////////////////////////////////////////////////////////
	// sdl config

	constexpr Uint32 flags = SDL_INIT_VIDEO;
	if (SDL_Init(flags) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", SDL_GetError());
		return -1;
	}

	{
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);

		SDL_Log("Compiled against SDL version %u.%u.%u ...\n", compiled.major, compiled.minor, compiled.patch);
		SDL_Log("Linking against SDL version %u.%u.%u.\n", linked.major, linked.minor, linked.patch);
	}

#if defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);	// Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  // was 0 in dear imgui example??
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

	auto* sdl_window = SDL_CreateWindow(
		"klotter",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		start_width,
		start_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);

	if (sdl_window == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
		return -1;
	}

	auto* sdl_gl_context = SDL_GL_CreateContext(sdl_window);

	if (sdl_gl_context == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create gl context: %s", SDL_GetError());

		SDL_DestroyWindow(sdl_window);
		sdl_window = nullptr;

		return -1;
	}

	SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);
	SDL_GL_SetSwapInterval(1);	// Enable vsync

	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to load OpenGL");

		SDL_GL_DeleteContext(sdl_gl_context);
		sdl_gl_context = nullptr;

		SDL_DestroyWindow(sdl_window);
		sdl_window = nullptr;

		return -1;
	}

	LOG_INFO("Loaded OpenGL %d.%d", GLVersion.major, GLVersion.minor);

	///////////////////////////////////////////////////////////////
	// load opengl debug
	setup_opengl_debug();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// dear imgui setup

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(sdl_window, sdl_gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);



	///////////////////////////////////////////////////////////////
	// run app
	const auto exit_code = app_main(rs, make_app, sdl_window);



	/////////////////////////////////////////////////////////////////////////////////////////
	// cleanup

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(sdl_gl_context);
	sdl_gl_context = nullptr;
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

	return exit_code;
}


}  //  namespace klotter
