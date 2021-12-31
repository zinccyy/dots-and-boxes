#include <SDL_events.h>
#include <SDL_video.h>
#include <game/state/pvp_state.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <utils/log.hpp>

#include <game/game.hpp>

#include <iostream>

#include <glm/gtx/string_cast.hpp>

namespace gm
{
namespace state
{
PlayerVsPlayerState::PlayerVsPlayerState(Game *game) : State(game)
{
}
int PlayerVsPlayerState::init()
{
    int error = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            error = mDots[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for dot %d", i * j);
                return -1;
            }

            // other stuff
            mDots[i][j].Size = 20;
            mDots[i][j].Hovered = false;
            mDots[i][j].Smoothstep = true;
            mDots[i][j].InnerColor = glm::vec3(242, 197, 149);
            mDots[i][j].OuterColor = glm::vec3(120, 120, 120);
            mDots[i][j].InnerRadius = 0.7;
            mDots[i][j].OuterRadius = 1;
        }
    }

    mRecalculateDotsPositions(glm::vec2(800, 600));

    utils::log::debug("loaded objects");

    error = mDotShaderProgram.loadShadersFromSource("assets/shaders/default/dot/vert.glsl", "assets/shaders/default/dot/frag.glsl");
    if (error)
    {
        utils::log::error("unable to load default shaders");
        return -1;
    }

    utils::log::debug("loaded shaders");

    glEnable(GL_DEPTH_TEST);

    return 0;
}
int PlayerVsPlayerState::processEvent(SDL_Event &event)
{
    int error = 0;

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

        utils::log::debug("mouse: %d:%d", x, y);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
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
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        // mouse pressed -> check for the hovered dot and draw a line from that dot up until the mouse position
        mPickedDot = mGetHoveredDot();
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        // mouse released -> check for hovered dot and connect if possible the picked and connect dots
        mConnectDot = mGetHoveredDot();
        if (mConnectDot != nullptr)
        {
            if (mPickedDot->Position == mConnectDot->Position)
            {
                // same dot -> do nothing, just release
                mPickedDot = nullptr;
                mConnectDot = nullptr;
            }
            else
            {
                // check if able to connect
            }
        }
    }

    return error;
}
int PlayerVsPlayerState::processInput()
{
    int error = 0;
    return error;
}
int PlayerVsPlayerState::draw()
{
    int error = 0;

    int x, y;
    glClearColor(0.470588, 0.470588, 0.470588, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto win_size = mGame->getWindowSize();
    const auto ratio = win_size.x / win_size.y;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            auto scale = glm::scale(glm::mat4(1), glm::vec3(glm::vec2(mDots[i][j].Size) / win_size, 0));
            auto translate = glm::translate(glm::mat4(1), glm::vec3(glm::vec2(mDots[i][j].Position.x * 2.0f - win_size.x, -mDots[i][j].Position.y * 2.0f + win_size.y) / (win_size), 0));

            mDotShaderProgram.use();
            mDotShaderProgram.setUniform("uScale", scale);
            mDotShaderProgram.setUniform("uTranslate", translate);

            mDots[i][j].draw(mDotShaderProgram);
        }
    }

    if (mPickedDot != nullptr)
    {
        // draw a line
    }

    return error;
}
void PlayerVsPlayerState::mRecalculateDotsPositions(const glm::vec2 &win_size)
{
    const auto w = win_size.x;
    const auto h = win_size.y;

    const auto step_w = w / 2;
    const auto step_h = h / 2;
    const auto start_point_w = step_w - step_w / 2;
    const auto start_point_h = step_h - step_h / 2;

    for (int i = 0; i < 3; i++)
    {
        auto w2 = start_point_w + i * start_point_w;
        for (int j = 0; j < 3; j++)
        {
            auto h2 = start_point_h + j * start_point_h;
            mDots[i][j].Position = glm::vec2(w2, h2);
        }
    }
}
eng::draw::Dot *PlayerVsPlayerState::mGetHoveredDot()
{
    eng::draw::Dot *ptr = nullptr;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (mDots[i][j].Hovered)
            {
                ptr = &mDots[i][j];
                break;
            }
        }
    }

    return ptr;
}
PlayerVsPlayerState::~PlayerVsPlayerState()
{
}
} // namespace state
} // namespace gm