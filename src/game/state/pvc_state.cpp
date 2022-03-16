#include <SDL_events.h>
#include <SDL_video.h>
#include <game/state/pvc_state.hpp>

#include <sstream>
#include <utility>
#include <utils/log.hpp>
#include <utils/gl/gl.hpp>

#include <game/game.hpp>

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

// imgui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

namespace gm
{
namespace state
{
PlayerVsCPUState::PlayerVsCPUState(Game *game)
    : State(game),
      mBoardState(nullptr), mPlayerScoreText{eng::draw::Text(mCharsMap, glm::vec2(mGame->getWindowSize().x / 2, 700)), eng::draw::Text(mCharsMap, glm::vec2(mGame->getWindowSize().x / 2, 100))}
{
}
PlayerVsCPUState::PlayerVsCPUState(Game *game, int n, int m, GameLevel level) : PlayerVsCPUState(game)
{
    mLevel = level;
    mBoardState = new BoardState(game, n + 1, m + 1, (int)mLevel + 1);
}
int PlayerVsCPUState::init()
{
    int error = 0;

    error = mBoardState->init();
    if (error)
    {
        utils::log::error("unable to setup board state: %d", error);
        return -1;
    }

    utils::log::debug("loaded objects");

    error = mFontCharacterShaderProgram.loadShadersFromSource("assets/shaders/font/vert.glsl", "assets/shaders/font/frag.glsl");
    if (error)
    {
        utils::log::error("unable to load default line shaders");
        return -1;
    }

    utils::log::debug("loaded shaders");

    // init freetype
    if (FT_Init_FreeType(&mFreeType))
    {
        utils::log::error("could not init FreeType library");
        return -1;
    }

    // load UI font
    if (FT_New_Face(mFreeType, "assets/fonts/Roboto-Regular.ttf", 0, &mRobotoFace))
    {
        utils::log::error("unable to load roboto font face");
        return -1;
    }
    FT_Set_Pixel_Sizes(mRobotoFace, 0, 48);

    utils::log::debug("loaded fonts");

    for (int c = 0; c < 128; c++)
    {
        mCharsMap[c] = eng::draw::Character(c, mRobotoFace);

        error = mCharsMap[c].setupBuffers();
        if (error)
        {
            utils::log::error("unable to create bitmap for character %c", (char)c);
            return -1;
        }
    }

    utils::log::debug("loaded needed character bitmaps");

    // setup initial text
    mPlayerScoreText[0].Color = mBoardState->PlayerColors[0];
    mPlayerScoreText[0].setText("0");
    mPlayerScoreText[0].Scale = 0.5;

    mPlayerScoreText[1].Color = mBoardState->PlayerColors[1];
    mPlayerScoreText[1].setText("0");
    mPlayerScoreText[1].Scale = 0.5;

    for (auto &text : mPlayerScoreText)
    {
        error = text.setupBuffers();
        if (error)
        {
            utils::log::error("unable to setup buffers player score text");
            return -1;
        }
    }

    mRecalculatePositions(mGame->getWindowSize());

    // enable for transparency
    glEnable(GL_DEPTH_TEST);

    // needed for glyphs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return error;
}
int PlayerVsCPUState::processEvent(SDL_Event &event)
{
    int error = 0;

    // ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            mRecalculatePositions(glm::vec2(event.window.data1, event.window.data2));
            glViewport(0, 0, event.window.data1, event.window.data2);
        }
    }

    // forward events to the board state
    if (mBoardState->StateData.GameOver)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
    else
    {
        error = mBoardState->processEvent(event);
        if (error)
        {
            utils::log::error("Error occured with board state processing event %d", event.type);
            return -1;
        }
    }

    return error;
}
int PlayerVsCPUState::processInput()
{
    int error = 0;

    if (mBoardState->StateData.GameOver)
    {
        std::string win_info;

        if (mBoardState->StateData.Scores[0] > mBoardState->StateData.Scores[1])
        {
            win_info = "You won!";
        }
        else if (mBoardState->StateData.Scores[0] < mBoardState->StateData.Scores[1])
        {
            win_info = "CPU won!";
        }
        else
        {
            win_info = "It\'s a draw!";
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(200, 200));

        ImGui::Begin("Game finished");
        ImGui::Text("%s", win_info.c_str());
        if (ImGui::Button("Restart", ImVec2(70, 20)))
        {
            utils::log::debug("restart the game: %dx%d", (int)mBoardState->N - 1, (int)mBoardState->M - 1);
            auto new_state = new PlayerVsCPUState(mGame, (int)mBoardState->N - 1, (int)mBoardState->M - 1, mLevel);

            error = new_state->init();
            if (error)
            {
                utils::log::debug("unable to create new PvP state");
                return error;
            }

            // TODO: test this more because of calling delete in popState() but member function is still on the stack..
            mGame->popState();
            mGame->pushState(new_state);
        }

        if (ImGui::Button("Main Menu", ImVec2(70, 20)))
        {
            utils::log::debug("returning to main menu");
            mGame->popState();
        }
        ImGui::End();
    }

    return error;
}
int PlayerVsCPUState::draw()
{
    int error = 0;

    // setup score text
    std::array<std::stringstream, 2> scores_text;
    for (int i = 0; i < scores_text.size(); i++)
    {
        scores_text[i] << (int)mBoardState->StateData.Scores[i];
        mPlayerScoreText[i].setText(scores_text[i].str());
    }

    glClearColor(0.470588, 0.470588, 0.470588, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // text
    for (auto &text : mPlayerScoreText)
    {
        text.draw(mFontCharacterShaderProgram);
    }

    // board
    mBoardState->draw();

    if (mBoardState->StateData.GameOver)
    {
        // menu for restart or main menu exit
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return error;
}
void PlayerVsCPUState::mRecalculatePositions(const glm::vec2 &win_size)
{
    for (auto &text : mPlayerScoreText)
    {
        text.windowResize(win_size);
    }
}
PlayerVsCPUState::~PlayerVsCPUState()
{
    // shutdown freetype
    delete mBoardState;

    FT_Done_Face(mRobotoFace);
    FT_Done_FreeType(mFreeType);
}
} // namespace state
} // namespace gm