#pragma once

#include <game/state.hpp>

namespace gm
{
namespace state
{
// use imgui for showing the UI and run the game from there on
class MainMenu : public State
{
  public:
    MainMenu(Game *game);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~MainMenu();

  private:
    void mDrawTextCentered(const char *text);
    bool mDrawButtonCentered(const char *text);

    bool mShowMainMenu;
    bool mShowAbout;
    bool mShowModes;
    bool mShowSettings;
};
} // namespace state
} // namespace gm