#pragma once

#include <game/state.hpp>
#include <game/state/pvc_state.hpp>

#include <glm/glm.hpp>

namespace gm
{
namespace state
{
// use imgui for showing the UI and run the game from there on
class TmpState : public State
{
  public:
    TmpState(Game *game);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~TmpState();

  private:
    bool mShowMainMenu;
    bool mShowAbout;
    bool mShowModes;
    bool mShowSettings;

    bool mShowVsPlayerSizePicker;
    bool mShowVsCPUSizePicker;

    glm::ivec2 mNewFieldSize;
    GameLevel mCPULevel;
};
} // namespace state
} // namespace gm
