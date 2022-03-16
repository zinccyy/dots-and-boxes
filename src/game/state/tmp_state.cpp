#include <SDL_video.h>
#include <game/game.hpp>
#include <game/state/tmp_state.hpp>
#include <utils/log.hpp>

// GL
#include <GL/glew.h>

// imgui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

// state
#include <game/state/pvp_state.hpp>
#include <game/state/pvc_state.hpp>

namespace gm
{
namespace state
{
TmpState::TmpState(Game *game) : State(game), mShowVsPlayerSizePicker(false), mShowVsCPUSizePicker(false), mNewFieldSize(2, 2), mCPULevel(GameLevel::Medium)
{
}

int TmpState::init()
{
    int error = 0;
    return error;
}

int TmpState::processEvent(SDL_Event &event)
{
    int error = 0;
    ImGui_ImplSDL2_ProcessEvent(&event);
    return error;
}

int TmpState::processInput()
{
    int error = 0;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(700, 200));

    ImGui::Begin("Game finished");
    if (ImGui::Button("Restart", ImVec2(70, 20)))
    {
    }

    if (ImGui::Button("Main Menu", ImVec2(70, 20)))
    {
    }
    ImGui::End();

    return error;
}

int TmpState::draw()
{
    int error = 0;

    glClearColor(0.0627f, 0.0627f, 0.0627f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return error;
}
TmpState::~TmpState()
{
}
} // namespace state
} // namespace gm
