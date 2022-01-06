#include <SDL_events.h>
#include <SDL_video.h>
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
#include <game/state/pvp_state.hpp>

namespace gm
{
Game::Game() : mWindow(nullptr), mGLContext(nullptr), mRunning(false), mCurrentState(nullptr), mMainMenuState(nullptr), mPVPState(nullptr), mModePlayingState(nullptr), mSettingsState(nullptr)
{
}

int Game::init()
{
    int error = 0;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        utils::log::error("unable to init SDL");
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    mWindow = SDL_CreateWindow("dots-and-boxes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    mGLContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_MakeCurrent(mWindow, mGLContext);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    mImGuiIO = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(mWindow, mGLContext);
    ImGui_ImplOpenGL3_Init("#version 130");

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
    mMainMenuState = new gm::state::MainMenu(this);
    mPVPState = new gm::state::PlayerVsPlayerState(this, 4, 5);

    // set current state
    mCurrentState = mPVPState;

    // init all states
    error = mCurrentState->init();

    return error;
}

int Game::run()
{
    int error = 0;
    while (mRunning)
    {
        // 1: poll events
        while (SDL_PollEvent(&mEvent))
        {
            mCurrentState->processEvent(mEvent);
            if (mEvent.type == SDL_QUIT)
            {
                mRunning = false;
            }
        }

        // 2: update according to FPS etc.
        mCurrentState->processInput();

        // 3: draw stuff
        mCurrentState->draw();

        // 4: swap buffers
        SDL_GL_SwapWindow(mWindow);
    }
    return error;
}
Game::~Game()
{
    // delete states
    if (mMainMenuState)
    {
        delete mMainMenuState;
    }

    if (mPVPState)
    {
        delete mPVPState;
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
