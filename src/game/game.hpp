#pragma once

#include <game/state.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// SDL2
#include <SDL2/SDL.h>

// imgui
#include <imgui.h>

// glm
#include <glm/glm.hpp>

namespace gm
{
class Game
{
  public:
    Game();

    // init SDL window and opengl context
    int init();

    // init GLEW + setup opengl
    int initOpenGLData();

    // run the game
    int run();

    inline ImFont *getUIFont() const
    {
        return mUIFont;
    }

    glm::vec2 getWindowSize()
    {
        int x, y;
        SDL_GetWindowSize(mWindow, &x, &y);
        return glm::vec2(x, y);
    }

    ~Game();

  private:
    // is the game still running
    bool mRunning;

    // currently running state of the game - set to main menu at the start and from there forward manipulate using the Game class API
    gm::State *mCurrentState;

    // all possible states:

    // main menu opened
    gm::State *mMainMenuState;

    // plaver vs player state
    gm::State *mPVPState;

    // settings edit
    gm::State *mSettingsState;

    // one of the picked modes being played
    gm::State *mModePlayingState;

    // SDL2 data
    SDL_Window *mWindow;
    SDL_GLContext mGLContext;
    SDL_Event mEvent;

    // imgui data
    ImGuiIO mImGuiIO;
    ImFont *mUIFont;
};
} // namespace gm
