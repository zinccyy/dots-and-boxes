#include "engine/drawable/dot.hpp"
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
    : State(game), mPickedDot(nullptr), mConnectDot(nullptr), mGameFinished(false), mNewLine(nullptr), mCurrentPlayer(0), mPickedIndex(-1),
      mConnectIndex(-1), mPlayerScoreText{eng::draw::Text(mCharsMap, glm::vec2(mGame->getWindowSize().x / 2, 700)), eng::draw::Text(mCharsMap, glm::vec2(mGame->getWindowSize().x / 2, 100))},
      mPlayerColors{glm::vec3(255, 255, 255), glm::vec3(0, 0, 0)}
{
}
PlayerVsCPUState::PlayerVsCPUState(Game *game, int n, int m, GameLevel level) : PlayerVsCPUState(game)
{
    mBoard = utils::ds::Board(n + 1, m + 1);
    mLevel = level;

    mDots.resize(n + 1);
    for (int i = 0; i < n + 1; i++)
    {
        mDots[i].resize(m + 1);
    }

    // N x M boxes can be drawn
    mBoxes.resize(n);
    for (int i = 0; i < n; i++)
    {
        mBoxes[i].resize(m);
    }

    // row lines
    mPlaceholderRowLines.resize(n + 1);
    for (int i = 0; i < n + 1; i++)
    {
        mPlaceholderRowLines[i].resize(m);
    }

    // column lines
    mPlaceholderColumnLines.resize(n);
    for (int i = 0; i < n; i++)
    {
        mPlaceholderColumnLines[i].resize(m + 1);
    }
}
int PlayerVsCPUState::init()
{
    int error = 0;

    // dots
    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            error = mDots[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for dot %d", i * j);
                return -1;
            }

            // other stuff
            mDots[i][j].Size = 10;
            mDots[i][j].Hovered = false;
            mDots[i][j].Smoothstep = true;
            mDots[i][j].InnerColor = utils::gl::parseHexRGB("#f2c595");
            mDots[i][j].OuterColor = glm::vec3(120, 120, 120);
            mDots[i][j].InnerRadius = 0.8;
            mDots[i][j].OuterRadius = 1;
        }
    }

    // boxes
    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            error = mBoxes[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for box %d", i * j);
                return -1;
            }
            mBoxes[i][j].Draw = false;
        }
    }

    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            error = mPlaceholderRowLines[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for placeholder line %d", i * j);
                return -1;
            }

            mPlaceholderRowLines[i][j].Height = 1.f;
            mPlaceholderRowLines[i][j].Color = utils::gl::parseHexRGB("#C0C0C0");
            mPlaceholderRowLines[i][j].ConnectedDots.first = &mDots[i][j];
            mPlaceholderRowLines[i][j].ConnectedDots.second = &mDots[i][j + 1];
        }
    }

    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            error = mPlaceholderColumnLines[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for placeholder line %d", i * j);
                return -1;
            }

            mPlaceholderColumnLines[i][j].Height = 1.f;
            mPlaceholderColumnLines[i][j].Color = utils::gl::parseHexRGB("#C0C0C0");
            mPlaceholderColumnLines[i][j].ConnectedDots.first = &mDots[i][j];
            mPlaceholderColumnLines[i][j].ConnectedDots.second = &mDots[i + 1][j];
        }
    }
    mRecalculateDotsPositions(mGame->getWindowSize());

    utils::log::debug("loaded objects");

    error = mDotShaderProgram.loadShadersFromSource("assets/shaders/default/dot/vert.glsl", "assets/shaders/default/dot/frag.glsl");
    if (error)
    {
        utils::log::error("unable to load default dot shaders");
        return -1;
    }

    error = mBoxShaderProgram.loadShadersFromSource("assets/shaders/default/box/vert.glsl", "assets/shaders/default/box/frag.glsl");
    if (error)
    {
        utils::log::error("unable to load default box shaders");
        return -1;
    }

    error = mLineShaderProgram.loadShadersFromSource("assets/shaders/default/line/vert.glsl", "assets/shaders/default/line/frag.glsl");
    if (error)
    {
        utils::log::error("unable to load default line shaders");
        return -1;
    }

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

    mPlayerColors[0] = utils::gl::parseHexRGB("#6495ed");
    mPlayerColors[1] = utils::gl::parseHexRGB("#f08080");

    utils::log::debug("loaded needed character bitmaps");

    // setup initial text
    mPlayerScoreText[0].Color = mPlayerColors[0];
    mPlayerScoreText[0].setText("0");
    mPlayerScoreText[0].Scale = 0.5;

    mPlayerScoreText[1].Color = mPlayerColors[1];
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

    // enable for transparency
    glEnable(GL_DEPTH_TEST);

    // needed for glyphs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}
int PlayerVsCPUState::processEvent(SDL_Event &event)
{
    int error = 0;

    if (mGameFinished)
    {
        // imgui
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
    else
    {
        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                mRecalculateDotsPositions(glm::vec2(event.window.data1, event.window.data2));
                glViewport(0, 0, event.window.data1, event.window.data2);
            }
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            auto x = event.motion.x;
            auto y = event.motion.y;

            auto xrel = event.motion.xrel;
            auto yrel = event.motion.yrel;

            for (int i = 0; i < mBoard.N; i++)
            {
                for (int j = 0; j < mBoard.M; j++)
                {
                    if (glm::distance(glm::vec2(x, y), mDots[i][j].Position) < mDots[i][j].Size)
                    {
                        mDots[i][j].Hovered = true;
                    }
                    else
                    {
                        mDots[i][j].Hovered = false;
                    }
                }
            }
            if (mNewLine && mPickedDot && !mConnectDot)
            {
                mNewLine->EndPosition = glm::vec2(event.motion.x, event.motion.y);
                mNewLine->windowResize(mGame->getWindowSize());
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            // mouse pressed -> check for the hovered dot and draw a line from that dot up until the mouse position
            utils::log::debug("mouse down");
            mPickedDot = mGetHoveredDot();
            if (mPickedDot != nullptr)
            {
                mNewLine = new eng::draw::Line();

                utils::log::debug("adding new line");
                utils::log::debug("setting line buffers");
                mNewLine->setupBuffers();

                if (error)
                {
                    utils::log::debug("unable to setup line buffers");
                    return -1;
                }
                utils::log::debug("setting properties");

                mNewLine->Height = 1.5f;
                mNewLine->Color = mPlayerColors[mCurrentPlayer];
                mNewLine->ConnectedDots.first = mPickedDot;
                mNewLine->StartPosition = mNewLine->EndPosition = mPickedDot->Position;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            // mouse released -> check for hovered dot and connect if possible the picked and connect dots
            utils::log::debug("mouse up");
            mConnectDot = mGetHoveredDot();
            if (mConnectDot != nullptr && mPickedDot != nullptr)
            {
                utils::log::debug("picked + connect valid - setting positions");
                if (mPickedDot->Position == mConnectDot->Position)
                {
                    // same dot -> do nothing, just release
                    mPickedDot = nullptr;
                    mConnectDot = nullptr;

                    utils::log::debug("same dot - ignore");
                    delete mNewLine;
                    mNewLine = nullptr;
                }
                else
                {
                    // check if able to connect
                    if ((mPickedDot->Position.x == mConnectDot->Position.x && mPickedDot->Position.y != mConnectDot->Position.y &&
                         glm::abs(glm::abs(mPickedDot->Position.y - mConnectDot->Position.y) - mDotsDistance.y) < 1.0f) ||
                        ((mPickedDot->Position.y == mConnectDot->Position.y && mPickedDot->Position.x != mConnectDot->Position.x &&
                          glm::abs(glm::abs(mPickedDot->Position.x - mConnectDot->Position.x) - mDotsDistance.x) < 1.0f)))
                    {
                        utils::log::debug("dots distance: %f", mDotsDistance.y);
                        utils::log::debug("dots real distance: %f", glm::abs(mPickedDot->Position.y - mConnectDot->Position.y));
                        utils::log::debug("normal line - adding fully to the collection");

                        mNewLine->EndPosition = mConnectDot->Position;
                        mNewLine->ConnectedDots.second = mConnectDot;
                        mNewLine->windowResize(mGame->getWindowSize());

                        // change color to black/gray after drawing permanently
                        mNewLine->Color = glm::vec3(52, 52, 52);
                        mLines.push_back(mNewLine);
                        mNewLine = nullptr;

                        utils::log::debug("picked = %d, connect = %d", mPickedIndex, mConnectIndex);
                        mBoard.move({mPickedIndex, mConnectIndex});

                        bool any_new = mCheckForNewBoxes();
                        if (!any_new)
                        {
                            mCurrentPlayer = !mCurrentPlayer;

                            // immediately after a player draws a line -> make CPU decide and draw a line
                            mCPUDrawLine();
                        }
                    }
                    else
                    {
                        utils::log::debug("unable to connect cross");
                        delete mNewLine;
                        mNewLine = nullptr;
                    }

                    mPickedDot = nullptr;
                    mConnectDot = nullptr;
                    mPickedIndex = -1;
                    mConnectIndex = -1;
                }
            }
            else if (mPickedDot != nullptr)
            {
                mPickedDot = nullptr;
                mConnectDot = nullptr;
                delete mNewLine;
                mNewLine = nullptr;
            }
            else if (mConnectDot != nullptr)
            {
                mPickedDot = nullptr;
                mConnectDot = nullptr;
                delete mNewLine;
                mNewLine = nullptr;
            }
        }
    }

    return error;
}
int PlayerVsCPUState::processInput()
{
    int error = 0;

    if (mGameFinished)
    {
        std::string win_info;

        if (mBoard.Scores[0] > mBoard.Scores[1])
        {
            win_info = "Player 1 won!";
        }
        else if (mBoard.Scores[0] < mBoard.Scores[1])
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

        ImGui::SetNextWindowSize(ImVec2(400, 200));

        ImGui::Begin("Game finished");
        ImGui::Text("%s", win_info.c_str());
        if (ImGui::Button("Restart", ImVec2(70, 20)))
        {
            utils::log::debug("restart the game: %dx%d", (int)mBoard.N - 1, (int)mBoard.M - 1);
            auto new_state = new PlayerVsCPUState(mGame, (int)mBoard.N - 1, (int)mBoard.M - 1, mLevel);

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

    int x, y;
    glClearColor(0.470588, 0.470588, 0.470588, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto win_size = mGame->getWindowSize();
    const auto ratio = win_size.x / win_size.y;

    // text
    for (auto &text : mPlayerScoreText)
    {
        text.draw(mFontCharacterShaderProgram);
    }

    // dots
    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            mDots[i][j].draw(mDotShaderProgram);
        }
    }

    // boxes
    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            mBoxes[i][j].draw(mBoxShaderProgram);
        }
    }

    // lines
    if (mNewLine != nullptr)
    {
        mNewLine->draw(mLineShaderProgram);
    }

    for (auto &line : mLines)
    {
        line->draw(mLineShaderProgram);
    }

    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            mPlaceholderRowLines[i][j].draw(mLineShaderProgram);
        }
    }

    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            mPlaceholderColumnLines[i][j].draw(mLineShaderProgram);
        }
    }

    if (mGameFinished)
    {
        // menu for restart or main menu exit
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return error;
}
void PlayerVsCPUState::mRecalculateDotsPositions(const glm::vec2 &win_size)
{
    const auto w = win_size.x;
    const auto h = win_size.y;

    const auto start_point_w = w / mBoard.M;
    const auto start_point_h = h / mBoard.N;
    const auto step_w = ((float)w - 2 * start_point_w) / (mBoard.M - 1);
    const auto step_h = ((float)h - 2 * start_point_h) / (mBoard.N - 1);

    mDotsDistance = glm::vec2(step_w, step_h);

    // dots
    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            mDots[i][j].Position = glm::vec2(start_point_w + j * step_w, start_point_h + i * step_h);
            mDots[i][j].windowResize(win_size);
        }
    }

    // boxes
    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            mBoxes[i][j].Position = glm::vec2((mDots[i][j].Position.x + mDots[i][j + 1].Position.x) / 2.f, (mDots[i][j].Position.y + mDots[i + 1][j].Position.y) / 2.f);
            mBoxes[i][j].Size = glm::vec2((mDots[i][j + 1].Position.x - mDots[i][j].Position.x) / 2.5f, (mDots[i + 1][j].Position.y - mDots[i][j].Position.y) / 2.5f);
            mBoxes[i][j].windowResize(win_size);
        }
    }

    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            mPlaceholderRowLines[i][j].updatePositions();
            mPlaceholderRowLines[i][j].windowResize(win_size);
        }
    }

    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            mPlaceholderColumnLines[i][j].updatePositions();
            mPlaceholderColumnLines[i][j].windowResize(win_size);
        }
    }

    for (auto &line : mLines)
    {
        line->updatePositions();
        line->windowResize(win_size);
    }

    // update text orthogonal matrices also
    for (auto &text : mPlayerScoreText)
    {
        text.windowResize(win_size);
    }
}
bool PlayerVsCPUState::mCheckForNewBoxes()
{
    bool any_new = false;
    std::stringstream score_ss;

    for (int i = 0; i < mBoard.N - 1; i++)
    {
        for (int j = 0; j < mBoard.M - 1; j++)
        {
            if (mBoxes[i][j].Draw)
            {
                // box is already drawn
                continue;
            }

            const int top_left = mBoard.M * i + j;
            const int top_right = mBoard.M * i + (j + 1);
            const int bottom_left = mBoard.M * (i + 1) + j;
            const int bottom_right = mBoard.M * (i + 1) + (j + 1);

            // check if a box is formed
            if (mBoard.AdjencyMatrix[top_left][top_right] && mBoard.AdjencyMatrix[top_left][bottom_left] && mBoard.AdjencyMatrix[top_right][bottom_right] &&
                mBoard.AdjencyMatrix[bottom_left][bottom_right])
            {
                mBoxes[i][j].Draw = true;
                mBoxes[i][j].Color = mPlayerColors[mCurrentPlayer];
                any_new = true;

                utils::log::debug("New box found (%d,%d,%d,%d)", top_left, top_right, bottom_left, bottom_right);

                // setup score
                mBoard.Scores[mCurrentPlayer]++;
                score_ss << mBoard.Scores[mCurrentPlayer];
                mPlayerScoreText[mCurrentPlayer].setText(score_ss.str());
                score_ss.str("");
            }
        }
    }

    if (mBoard.Scores[0] + mBoard.Scores[1] == ((mBoard.N - 1) * (mBoard.M - 1)))
    {
        // all the boxes have been drawn -> game over -> print a winner -> use imgui on next render
        mGameFinished = true;
    }
    return any_new;
}
int PlayerVsCPUState::mCPUDrawLine()
{
    int error = 0;

    mNewLine = new eng::draw::Line();

    utils::log::debug("adding new line");
    utils::log::debug("setting line buffers");
    mNewLine->setupBuffers();

    if (error)
    {
        utils::log::debug("unable to setup line buffers");
        return -1;
    }
    utils::log::debug("setting properties");

    mNewLine->Height = 1.5f;
    mNewLine->Color = glm::vec3(52, 52, 52);

    // use minimax algorithm to draw best possible line
    auto line = mBoard.minimax();

    mPickedIndex = line.first;
    mConnectIndex = line.second;

    auto mp_row = mPickedIndex / (int)mBoard.N;
    auto mp_col = mPickedIndex % (int)mBoard.M;
    auto mc_row = mConnectIndex / (int)mBoard.N;
    auto mc_col = mConnectIndex % (int)mBoard.M;

    utils::log::debug("coordinates to connect : (%d, %d), (%d, %d)", mp_row, mp_col, mc_row, mc_col);

    mNewLine->ConnectedDots.first = &mDots[mp_row][mp_col];
    mNewLine->ConnectedDots.second = &mDots[mc_row][mc_col];

    mNewLine->StartPosition = mNewLine->ConnectedDots.first->Position;
    mNewLine->EndPosition = mNewLine->ConnectedDots.second->Position;

    mNewLine->windowResize(mGame->getWindowSize());

    // add to lines
    mLines.push_back(mNewLine);
    mNewLine = nullptr;

    utils::log::debug("picked = %d, connect = %d", line.first, line.second);

    bool any_new = mCheckForNewBoxes();
    if (!any_new)
    {
        mCurrentPlayer = !mCurrentPlayer;
    }
    else
    {
        // keep drawing until no new box has been found
        mCPUDrawLine();
    }

    return error;
}
eng::draw::Dot *PlayerVsCPUState::mGetHoveredDot()
{
    eng::draw::Dot *ptr = nullptr;

    for (int i = 0; i < mBoard.N; i++)
    {
        for (int j = 0; j < mBoard.M; j++)
        {
            if (mDots[i][j].Hovered)
            {
                ptr = &mDots[i][j];
                if (!mPickedDot)
                {
                    mPickedIndex = i * mBoard.M + j;
                }
                else if (mPickedDot && !mConnectDot)
                {
                    mConnectIndex = i * mBoard.M + j;
                }
                break;
            }
        }
    }

    return ptr;
}
PlayerVsCPUState::~PlayerVsCPUState()
{
    for (auto &line : mLines)
    {
        delete line;
    }
    if (mNewLine)
    {
        delete mNewLine;
        mNewLine = nullptr;
    }

    // shutdown freetype
    FT_Done_Face(mRobotoFace);
    FT_Done_FreeType(mFreeType);
}
} // namespace state
} // namespace gm