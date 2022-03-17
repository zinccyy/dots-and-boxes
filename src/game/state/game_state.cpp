#include <SDL_events.h>
#include <SDL_video.h>
#include <game/state/game_state.hpp>

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
GameState::GameState(Game *game)
    : State(game), mBoardState(nullptr),
      mRestartMenuReady(false), mPlayerScoreText{eng::draw::Text(mCharsMap, glm::vec2(0, 0)), eng::draw::Text(mCharsMap, glm::vec2(0, 0))}, mPlayerNameText{eng::draw::Text(mCharsMap, glm::vec2(0, 0)),
                                                                                                                                                            eng::draw::Text(mCharsMap, glm::vec2(0, 0))}
{
}
GameState::GameState(Game *game, int n, int m, GameLevel level) : GameState(game)
{
    mLevel = level;
    mBoardState = new BoardState(game, n + 1, m + 1, (int)mLevel + 1);
}
int GameState::init()
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
    for (int i = 0; i < mPlayerScoreText.size(); i++)
    {
        mPlayerScoreText[i].Color = mBoardState->PlayerColors[i];
        mPlayerScoreText[i].setText("0");
        mPlayerScoreText[i].Scale = 0.5;

        mPlayerNameText[i].Color = glm::vec3(53, 53, 53);
        mPlayerNameText[i].Scale = 0.5;
    }

    for (auto &text : mPlayerScoreText)
    {
        error = text.setupBuffers();
        if (error)
        {
            utils::log::error("unable to setup buffers player score text");
            return -1;
        }
    }

    if (mLevel == GameLevel::None)
    {
        mPlayerNameText[0].setText("Player 1: ");
        mPlayerNameText[1].setText("Player 2: ");
    }
    else
    {
        mPlayerNameText[0].setText("You: ");
        mPlayerNameText[1].setText("CPU: ");
    }

    for (auto &text : mPlayerNameText)
    {
        error = text.setupBuffers();
        if (error)
        {
            utils::log::error("unable to setup buffers player name text");
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
int GameState::processEvent(SDL_Event &event)
{
    int error = 0;

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
int GameState::processInput()
{
    int error = 0;

    if (mBoardState->StateData.GameOver)
    {
        mRestartMenuReady = true;
        std::string win_info;

        if (mBoardState->StateData.Scores[0] > mBoardState->StateData.Scores[1])
        {
            if (mLevel == GameLevel::None)
            {
                win_info = "Player 1 won!";
            }
            else
            {
                win_info = "You won!";
            }
        }
        else if (mBoardState->StateData.Scores[0] < mBoardState->StateData.Scores[1])
        {
            if (mLevel == GameLevel::None)
            {
                win_info = "Player 2 won!";
            }
            else
            {
                win_info = "CPU won!";
            }
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
            auto new_state = new GameState(mGame, (int)mBoardState->N - 1, (int)mBoardState->M - 1, mLevel);

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
    else
    {
        error = mBoardState->processInput();
    }

    return error;
}
int GameState::draw()
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

    for (auto &text : mPlayerNameText)
    {
        text.draw(mFontCharacterShaderProgram);
    }

    // board
    mBoardState->draw();

    if (mRestartMenuReady)
    {
        // menu for restart or main menu exit
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return error;
}
void GameState::mRecalculatePositions(const glm::vec2 &win_size)
{
    // recalculate positions for text based on dots Y positions
    for (auto &text : mPlayerNameText)
    {
        text.StartPosition.x = win_size.x / 8;
    }

    for (auto &text : mPlayerScoreText)
    {
        text.StartPosition.x = win_size.x / 8 + 100;
    }

    mPlayerScoreText[0].StartPosition.y = mBoardState->Dots[mBoardState->N - 1][0].Position.y + 200;
    mPlayerScoreText[1].StartPosition.y = mBoardState->Dots[mBoardState->N - 1][0].Position.y + 100;

    mPlayerNameText[0].StartPosition.y = mBoardState->Dots[mBoardState->N - 1][0].Position.y + 200;
    mPlayerNameText[1].StartPosition.y = mBoardState->Dots[mBoardState->N - 1][0].Position.y + 100;

    for (auto &text : mPlayerScoreText)
    {
        text.windowResize(win_size);
    }

    for (auto &text : mPlayerNameText)
    {
        text.windowResize(win_size);
    }
}
GameState::~GameState()
{
    // shutdown freetype
    delete mBoardState;

    FT_Done_Face(mRobotoFace);
    FT_Done_FreeType(mFreeType);
}
} // namespace state
} // namespace gm