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

// stdlib
#include <stack>

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

    void pushState(State *state)
    {
        mStates.push(state);
    }

    State *popState()
    {
        auto top = mStates.top();

        mStates.pop();

        return top;
    }

    ~Game();

  private:
    // is the game still running
    bool mRunning;

    // SDL2 data
    SDL_Window *mWindow;
    SDL_GLContext mGLContext;
    SDL_Event mEvent;

    // imgui data
    ImGuiIO mImGuiIO;
    ImFont *mUIFont;

    // state stack -> the top state is the current one
    std::stack<gm::State *> mStates;
};
} // namespace gm
