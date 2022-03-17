#include <SDL_video.h>
#include <game/game.hpp>
#include <game/state/main_menu.hpp>
#include <utils/log.hpp>

// GL
#include <GL/glew.h>

// imgui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

// state
#include <game/state/game_state.hpp>

namespace gm
{
namespace state
{
MainMenu::MainMenu(Game *game) : State(game), mShowVsPlayerSizePicker(false), mShowVsCPUSizePicker(false), mNewFieldSize(2, 2), mCPULevel(GameLevel::Medium)
{
}

int MainMenu::init()
{
    int error = 0;
    return error;
}

int MainMenu::processEvent(SDL_Event &event)
{
    int error = 0;
    ImGui_ImplSDL2_ProcessEvent(&event);
    return error;
}

int MainMenu::processInput()
{
    int error = 0;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    float f = 0.0f;
    int counter = 0;

    auto viewport = ImGui::GetMainViewport();
    auto font = mGame->getUIFont();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x / 2 - 200, viewport->WorkSize.y / 2 - 100));
    ImGui::SetNextWindowSize(ImVec2(400, 200));

    ImGui::Begin("MainMenuWindow", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar);

    auto title = "dNb - Dots and Boxes";
    auto description = "The Dots and Boxes game implemented in C++/OpenGL";

    // title + description
    mDrawTextCentered(title);
    ImGui::SetCursorPosY(30);
    mDrawTextCentered(description);

    // buttons
    ImGui::SetCursorPosY(60);

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(233, 84, 32, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(233 / 2, 84 / 2, 32 / 2, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(16, 16, 16, 255));
    if (mDrawButtonCentered("About"))
    {
        utils::log::debug("Show game about");
    }
    if (mDrawButtonCentered("vs Player"))
    {
        mShowVsPlayerSizePicker = true;
    }
    if (mDrawButtonCentered("vs CPU"))
    {
        mShowVsCPUSizePicker = true;
    }
    if (mDrawButtonCentered("Settings"))
    {
    }
    ImGui::PopStyleColor(3);
    ImGui::End();

    if (mShowVsPlayerSizePicker)
    {
        // get size of the field and start the game mode
        ImGui::Begin("Field size");
        ImGui::Text("Choose field size: ");
        ImGui::SliderInt("N", &mNewFieldSize.x, 2, 6);
        ImGui::SliderInt("M", &mNewFieldSize.y, 2, 6);
        if (ImGui::Button("Start", ImVec2(50, 20)))
        {
            utils::log::debug("start game: %dx%d", mNewFieldSize.x, mNewFieldSize.y);
            auto new_state = new gm::state::GameState(mGame, mNewFieldSize.x, mNewFieldSize.y, GameLevel::None);
            if (new_state->init())
            {
                // error
                error = -1;
            }
            else
            {
                mGame->pushState(new_state);
            }

            // remove after starting new game -> do not show after returning to the main menu
            mShowVsPlayerSizePicker = false;
        }
        ImGui::End();
    }

    if (mShowVsCPUSizePicker)
    {
        // get size of the field and start the game mode
        ImGui::Begin("Field size");
        ImGui::Text("Choose field size: ");
        ImGui::SliderInt("N", &mNewFieldSize.x, 2, 6);
        ImGui::SliderInt("M", &mNewFieldSize.y, 2, 6);
        const char *level_names[] = {
            "Easy",
            "Medium",
            "Hard",
        };
        ImGui::SliderInt("Level", (int *)&mCPULevel, 0, 2, level_names[(int)mCPULevel]);
        if (ImGui::Button("Start", ImVec2(50, 20)))
        {
            utils::log::debug("start game: %dx%d", mNewFieldSize.x, mNewFieldSize.y);
            // auto new_state = new gm::state::TmpState(mGame);
            auto new_state = new gm::state::GameState(mGame, mNewFieldSize.x, mNewFieldSize.y, mCPULevel);
            if (new_state->init())
            {
                // error
                error = -1;
            }
            else
            {
                mGame->pushState(new_state);
            }

            // remove after starting new game -> do not show after returning to the main menu
            mShowVsCPUSizePicker = false;
        }
        ImGui::End();
    }
    return error;
}

void MainMenu::mDrawTextCentered(const char *text)
{
    auto window_width = ImGui::GetWindowSize().x;
    auto text_width = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", text);
}

bool MainMenu::mDrawButtonCentered(const char *text)
{
    auto window_width = ImGui::GetWindowSize().x;
    auto text_width = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX((window_width - 200) * 0.5f);
    return ImGui::Button(text, ImVec2(200, 30));
}

int MainMenu::draw()
{
    int error = 0;

    glClearColor(0.0627f, 0.0627f, 0.0627f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return error;
}
MainMenu::~MainMenu()
{
}
} // namespace state
} // namespace gm
