#pragma once

#include <SDL2/SDL.h>

namespace gm
{
// predefine class
class Game;
// abstract state class
class State
{
  public:
    State(Game *game) : mGame(game)
    {
    }

    virtual int init() = 0;
    virtual int processEvent(SDL_Event &event) = 0;
    virtual int processInput() = 0;
    virtual int draw() = 0;

    virtual ~State()
    {
    }

  protected:
    Game *mGame;
};
} // namespace gm
