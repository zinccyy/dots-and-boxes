#include <SDL_events.h>
#include <SDL_video.h>
#include <ft2build.h>
#ifdef __EMSCRIPTEN__
#include FT_FREETYPE_H
#include <freetype.h>
#else
#include <freetype2/freetype/freetype.h>
#endif
#include <game/game.hpp>
#include <utils/log.hpp>

// GL
#include <GL/glew.h>

// imgui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

// states
#include <game/state/main_menu.hpp>

namespace gm
{
Game::Game() : mWindow(nullptr), mGLContext(nullptr), mRunning(false)
{
}

int Game::init()
{
    int error = 0;
#ifndef __EMSCRIPTEN__
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        utils::log::error("unable to init SDL");
        return -1;
    }
#endif

#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    mWindow = SDL_CreateWindow("dots-and-boxes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    mGLContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_MakeCurrent(mWindow, mGLContext);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    mImGuiIO = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(mWindow, mGLContext);
    ImGui_ImplOpenGL3_Init("#version 100");

    // starting the game
    mRunning = true;

    return error;
}

int Game::initOpenGLData()
{
    int error = 0;
    if (glewInit() != GLEW_OK)
    {
        utils::log::error("unable to init GLEW");
        return -1;
    }

    // setup states
    auto main_menu = new gm::state::MainMenu(this);
    error = main_menu->init();
    if (error)
    {
        utils::log::error("unable to init main menu state");
        return -1;
    }

    mStates.push(main_menu);

    return error;
}

bool Game::getRunningState()
{
    return this->mRunning;
}

void Game::loopIter()
{
    auto top_state = mStates.top();

    // 1: poll events
    while (SDL_PollEvent(&mEvent))
    {
        top_state->processEvent(mEvent);
        if (mEvent.type == SDL_QUIT)
        {
            mRunning = false;
        }
    }

    // 2: update according to FPS etc.
    top_state->processInput();

    // 3: draw stuff
    top_state->draw();

    // 4: swap buffers
    SDL_GL_SwapWindow(mWindow);
}

int Game::run()
{
    int error = 0;
    while (mRunning)
    {
        auto top_state = mStates.top();

        // 1: poll events
        while (SDL_PollEvent(&mEvent))
        {
            top_state->processEvent(mEvent);
            if (mEvent.type == SDL_QUIT)
            {
                mRunning = false;
            }
        }

        // 2: update according to FPS etc.
        top_state->processInput();

        // 3: draw stuff
        top_state->draw();

        // 4: swap buffers
        SDL_GL_SwapWindow(mWindow);
    }
    return error;
}
Game::~Game()
{
    while (mStates.size())
    {
        auto state = mStates.top();

        // free state data
        delete state;

        mStates.pop();
    }

    // shutdown imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // shutdown SDL
    SDL_GL_DeleteContext(mGLContext);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
} // namespace gm
