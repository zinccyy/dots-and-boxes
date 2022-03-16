#include <game/state/board_state.hpp>
#include <game/game.hpp>

#include <utils/gl/gl.hpp>
#include <utils/log.hpp>

namespace gm
{
namespace state
{
void BoardStateData::pickLine(std::pair<uint8_t, uint8_t> line)
{
    // lines
    AvailableLines.remove(line);
    AvailableLines.remove({line.second, line.first});

    // matrix
    AdjencyMatrix[line.first][line.second] = true;
    AdjencyMatrix[line.second][line.first] = true;
}
bool BoardStateData::checkForNewBoxes(int N, int M)
{
    bool any_new = false;

    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < M; j++)
    //     {
    //         printf("%d ", AdjencyMatrix[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("\n");

    // for (int i = 0; i < N - 1; i++)
    // {
    //     for (int j = 0; j < M - 1; j++)
    //     {
    //         printf("%d ", BoxesDraw[i][j]);
    //     }
    //     printf("\n");
    // }

    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < M - 1; j++)
        {
            if (BoxesDraw[i][j])
            {
                // box is already drawn
                continue;
            }

            const int top_left = M * i + j;
            const int top_right = M * i + (j + 1);
            const int bottom_left = M * (i + 1) + j;
            const int bottom_right = M * (i + 1) + (j + 1);

            // check if a box is formed
            if (AdjencyMatrix[top_left][top_right] && AdjencyMatrix[top_left][bottom_left] && AdjencyMatrix[top_right][bottom_right] && AdjencyMatrix[bottom_left][bottom_right])
            {
                BoxesDraw[i][j] = true;
                any_new = true;

                utils::log::debug("New box found (%d,%d,%d,%d)", top_left, top_right, bottom_left, bottom_right);

                // setup score
                Scores[CurrentPlayer]++;
            }
        }
    }

    if (Scores[0] + Scores[1] == ((N - 1) * (M - 1)))
    {
        // all the boxes have been drawn -> game over -> print a winner -> use imgui on next render
        utils::log::debug("Game over!");
        GameOver = true;
    }

    return any_new;
}
BoardState::BoardState(Game *game) : State(game), PickedDot({-1, nullptr}), ConnectDot({-1, nullptr}), NewLine(nullptr), DotsDistance(0, 0)
{
}
BoardState::BoardState(Game *game, int n, int m) : BoardState(game)
{
    N = n;
    M = m;

    // setup colors
    PlayerColors[0] = utils::gl::parseHexRGB("#6495ed");
    PlayerColors[1] = utils::gl::parseHexRGB("#f08080");

    // setup state data
    for (int i = 0; i < this->N - 1; i++)
    {
        for (int j = 0; j < this->M - 1; j++)
        {
            const int top_left = this->M * i + j;
            const int top_right = this->M * i + (j + 1);
            const int bottom_left = this->M * (i + 1) + j;
            const int bottom_right = this->M * (i + 1) + (j + 1);

            StateData.AvailableLines.push_back(std::make_pair(top_left, top_right));
            StateData.AvailableLines.push_back(std::make_pair(top_left, bottom_left));

            if (j == this->M - 2)
            {
                // last col -> add right side
                StateData.AvailableLines.push_back(std::make_pair(top_right, bottom_right));
                // utils::log::debug("line added: (%d, %d)", top_right, bottom_right);
            }

            if (i == this->N - 2)
            {
                // last row -> add bottom side
                StateData.AvailableLines.push_back(std::make_pair(bottom_left, bottom_right));
                // utils::log::debug("line added: (%d, %d)", bottom_left, bottom_right);
            }
        }
    }
}
int BoardState::init()
{
    int error = 0;

    utils::log::info("Loading objects");

    // dots
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            error = Dots[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for dot %d", i * j);
                return -1;
            }

            // other stuff
            Dots[i][j].Size = 10;
            Dots[i][j].Hovered = false;
            Dots[i][j].Smoothstep = true;
            Dots[i][j].InnerColor = utils::gl::parseHexRGB("#f2c595");
            Dots[i][j].OuterColor = glm::vec3(120, 120, 120);
            Dots[i][j].InnerRadius = 0.8;
            Dots[i][j].OuterRadius = 1;
        }
    }

    // boxes
    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < M - 1; j++)
        {
            error = Boxes[i][j].setupBuffers();
            if (error)
            {
                utils::log::error("unable to setup buffers for box %d", i * j);
                return -1;
            }
            StateData.BoxesDraw[i][j] = false;
        }
    }

    // placeholder lines
    for (int i = 0; i < N; i++)
    {
        error = mPlaceholderRowLines[i].setupBuffers();
        if (error)
        {
            utils::log::error("unable to setup buffers for placeholder row line %d", i);
            return -1;
        }

        mPlaceholderRowLines[i].Height = 1.f;
        mPlaceholderRowLines[i].Color = utils::gl::parseHexRGB("#C0C0C0");
        mPlaceholderRowLines[i].ConnectedDots.first = &Dots[i][0];
        mPlaceholderRowLines[i].ConnectedDots.second = &Dots[i][M - 1];
    }
    for (int i = 0; i < M; i++)
    {
        error = mPlaceholderColLines[i].setupBuffers();
        if (error)
        {
            utils::log::error("unable to setup buffers for placeholder col line %d", i);
            return -1;
        }

        mPlaceholderColLines[i].Height = 1.f;
        mPlaceholderColLines[i].Color = utils::gl::parseHexRGB("#C0C0C0");
        mPlaceholderColLines[i].ConnectedDots.first = &Dots[0][i];
        mPlaceholderColLines[i].ConnectedDots.second = &Dots[N - 1][i];
    }

    mRecalculatePositions(mGame->getWindowSize());

    utils::log::info("Loading shaders");

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

    return error;
}
int BoardState::processEvent(SDL_Event &event)
{
    int error = 0;
    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            mRecalculatePositions(glm::vec2(event.window.data1, event.window.data2));
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        auto x = event.motion.x;
        auto y = event.motion.y;

        auto xrel = event.motion.xrel;
        auto yrel = event.motion.yrel;

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                if (glm::distance(glm::vec2(x, y), Dots[i][j].Position) < Dots[i][j].Size)
                {
                    Dots[i][j].Hovered = true;
                }
                else
                {
                    Dots[i][j].Hovered = false;
                }
            }
        }
        if (NewLine && PickedDot.second && !ConnectDot.second)
        {
            NewLine->EndPosition = glm::vec2(event.motion.x, event.motion.y);
            NewLine->windowResize(mGame->getWindowSize());
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        // mouse pressed -> check for the hovered dot and draw a line from that dot up until the mouse position
        utils::log::debug("mouse down");
        mGetHoveredDot();
        if (PickedDot.second != nullptr)
        {
            NewLine = new eng::draw::Line();

            utils::log::debug("adding new line");
            utils::log::debug("setting line buffers");
            NewLine->setupBuffers();

            if (error)
            {
                utils::log::debug("unable to setup line buffers");
                return -1;
            }
            utils::log::debug("setting properties");

            NewLine->Height = 1.5f;
            NewLine->Color = PlayerColors[StateData.CurrentPlayer];
            NewLine->ConnectedDots.first = PickedDot.second;
            NewLine->StartPosition = NewLine->EndPosition = PickedDot.second->Position;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        // mouse released -> check for hovered dot and connect if possible the picked and connect dots
        utils::log::debug("mouse up");
        mGetHoveredDot();
        if (ConnectDot.second != nullptr && PickedDot.second != nullptr)
        {
            utils::log::debug("picked + connect valid - setting positions");
            if (PickedDot.second->Position == ConnectDot.second->Position)
            {
                // same dot -> do nothing, just release
                PickedDot.second = nullptr;
                ConnectDot.second = nullptr;

                utils::log::debug("same dot - ignore");
                delete NewLine;
                NewLine = nullptr;
            }
            else
            {
                // check if able to connect
                if ((PickedDot.second->Position.x == ConnectDot.second->Position.x && PickedDot.second->Position.y != ConnectDot.second->Position.y &&
                     glm::abs(glm::abs(PickedDot.second->Position.y - ConnectDot.second->Position.y) - DotsDistance.y) < 1.0f) ||
                    ((PickedDot.second->Position.y == ConnectDot.second->Position.y && PickedDot.second->Position.x != ConnectDot.second->Position.x &&
                      glm::abs(glm::abs(PickedDot.second->Position.x - ConnectDot.second->Position.x) - DotsDistance.x) < 1.0f)))
                {
                    // utils::log::debug("dots distance: %f", DotsDistance.y);
                    // utils::log::debug("dots real distance: %f", glm::abs(PickedDot.second->Position.y - ConnectDot.second->Position.y));
                    // utils::log::debug("normal line - adding fully to the collection");

                    NewLine->EndPosition = ConnectDot.second->Position;
                    NewLine->ConnectedDots.second = ConnectDot.second;
                    NewLine->windowResize(mGame->getWindowSize());

                    // change color to black/gray after drawing permanently
                    NewLine->Color = glm::vec3(52, 52, 52);
                    Lines.push_back(NewLine);
                    NewLine = nullptr;

                    utils::log::debug("picked = %d, connect = %d", PickedDot.first, ConnectDot.first);

                    StateData.pickLine({PickedDot.first, ConnectDot.first});
                    bool any_new = StateData.checkForNewBoxes(N, M);
                    if (!any_new)
                    {
                        StateData.CurrentPlayer = !StateData.CurrentPlayer;
                        // mCPUDrawLine();
                    }
                    else
                    {
                        // assign color to the new boxes
                        for (int i = 0; i < N - 1; i++)
                        {
                            for (int j = 0; j < M - 1; j++)
                            {
                                if (StateData.BoxesDraw[i][j] && !Boxes[i][j].Draw)
                                {
                                    Boxes[i][j].Draw = true;
                                    Boxes[i][j].Color = PlayerColors[StateData.CurrentPlayer];
                                }
                            }
                        }
                    }
                }
                else
                {
                    utils::log::debug("unable to connect cross");
                    delete NewLine;
                    NewLine = nullptr;
                }

                PickedDot.second = nullptr;
                ConnectDot.second = nullptr;
                PickedDot.first = -1;
                ConnectDot.first = -1;
            }
        }
        else if (PickedDot.second != nullptr)
        {
            PickedDot.second = nullptr;
            ConnectDot.second = nullptr;
            delete NewLine;
            NewLine = nullptr;
        }
        else if (ConnectDot.second != nullptr)
        {
            PickedDot.second = nullptr;
            ConnectDot.second = nullptr;
            delete NewLine;
            NewLine = nullptr;
        }
    }
    return error;
}
int BoardState::processInput()
{
    int error = 0;
    return error;
}
int BoardState::draw()
{
    int error = 0;

    // dots
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            Dots[i][j].draw(mDotShaderProgram);
        }
    }

    // boxes
    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < M - 1; j++)
        {
            if (StateData.BoxesDraw[i][j])
            {
                Boxes[i][j].draw(mBoxShaderProgram);
            }
        }
    }

    // lines
    if (NewLine != nullptr)
    {
        NewLine->draw(mLineShaderProgram);
    }

    for (auto &line : Lines)
    {
        line->draw(mLineShaderProgram);
    }

    // placeholder lines
    for (int i = 0; i < N; i++)
    {
        mPlaceholderRowLines[i].draw(mLineShaderProgram);
    }
    for (int i = 0; i < M; i++)
    {
        mPlaceholderColLines[i].draw(mLineShaderProgram);
    }

    return error;
}
void BoardState::mRecalculatePositions(const glm::vec2 &win_size)
{
    const auto w = win_size.x;
    const auto h = win_size.y;

    const auto start_point_w = w / M;
    const auto start_point_h = h / N;
    const auto step_w = ((float)w - 2 * start_point_w) / (M - 1);
    const auto step_h = ((float)h - 2 * start_point_h) / (N - 1);

    DotsDistance = glm::vec2(step_w, step_h);

    // dots
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            Dots[i][j].Position = glm::vec2(start_point_w + j * step_w, start_point_h + i * step_h);
            Dots[i][j].windowResize(win_size);
        }
    }

    // boxes
    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < M - 1; j++)
        {
            Boxes[i][j].Position = glm::vec2((Dots[i][j].Position.x + Dots[i][j + 1].Position.x) / 2.f, (Dots[i][j].Position.y + Dots[i + 1][j].Position.y) / 2.f);
            Boxes[i][j].Size = glm::vec2((Dots[i][j + 1].Position.x - Dots[i][j].Position.x) / 2.5f, (Dots[i + 1][j].Position.y - Dots[i][j].Position.y) / 2.5f);
            Boxes[i][j].windowResize(win_size);
        }
    }

    // placeholder lines
    for (int i = 0; i < N; i++)
    {
        mPlaceholderRowLines[i].updatePositions();
        mPlaceholderRowLines[i].windowResize(win_size);
    }
    for (int i = 0; i < M; i++)
    {
        mPlaceholderColLines[i].updatePositions();
        mPlaceholderColLines[i].windowResize(win_size);
    }

    // lines
    for (auto &line : Lines)
    {
        line->updatePositions();
        line->windowResize(win_size);
    }
}
int BoardState::mHeuristicValue(BoardStateData &state)
{
    return state.Scores[1] - state.Scores[0];
}
std::pair<LineIndices, uint8_t> BoardState::mMiniMax(BoardStateData &state, uint8_t depth, uint8_t alpha, uint8_t beta)
{
    std::pair<LineIndices, int8_t> move = {{-1, -1}, 0};

    if (depth == 0 || state.GameOver)
    {
        move.second = mHeuristicValue(state);
        return move;
    }

    if (state.CurrentPlayer == true)
    {
        // CPU
        move.second = -100;
        for (auto line : state.AvailableLines)
        {
            auto new_state = state;
            new_state.pickLine(line);
            auto new_boxes = new_state.checkForNewBoxes(N, M);
            if (!new_boxes)
            {
                new_state.CurrentPlayer = !new_state.CurrentPlayer;
            }

            auto eval = mMiniMax(new_state, depth - 1, alpha, beta);
            if (eval.second > move.second)
            {
                move = {line, eval.second};
            }
            alpha = std::max(alpha, eval.second);
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    else
    {
        // player
        move.second = 100;

        for (auto line : state.AvailableLines)
        {
            auto new_state = state;
            new_state.pickLine(line);
            auto new_boxes = new_state.checkForNewBoxes(N, M);
            if (!new_boxes)
            {
                new_state.CurrentPlayer = !new_state.CurrentPlayer;
            }

            auto eval = mMiniMax(new_state, depth - 1, alpha, beta);
            if (eval.second < move.second)
            {
                move = {line, eval.second};
            }
            beta = std::min(beta, eval.second);
            if (beta <= alpha)
            {
                break;
            }
        }
    }

    return move;
}
int BoardState::mCPUDrawLine()
{
    int error = 0;

    NewLine = new eng::draw::Line();

    utils::log::debug("adding new line");
    utils::log::debug("setting line buffers");
    error = NewLine->setupBuffers();

    if (error)
    {
        utils::log::debug("unable to setup line buffers");
        return -1;
    }
    utils::log::debug("setting properties");

    NewLine->Height = 1.5f;
    NewLine->Color = glm::vec3(52, 52, 52);

    // use minimax algorithm to draw best possible line
    auto line = mMiniMax(StateData, 2, -100, 100).first;

    if (line == LineIndices{-1, -1})
    {
        line = StateData.AvailableLines.back();
    }

    StateData.pickLine(line);

    PickedDot.first = line.first;
    ConnectDot.first = line.second;

    auto mp_row = (int)PickedDot.first / (int)M;
    auto mp_col = (int)PickedDot.first % (int)M;
    auto mc_row = (int)ConnectDot.first / (int)M;
    auto mc_col = (int)ConnectDot.first % (int)M;

    utils::log::debug("coordinates to connect : (%d, %d), (%d, %d)", mp_row, mp_col, mc_row, mc_col);

    NewLine->ConnectedDots.first = &Dots[mp_row][mp_col];
    NewLine->ConnectedDots.second = &Dots[mc_row][mc_col];

    NewLine->updatePositions();

    NewLine->windowResize(mGame->getWindowSize());

    // add to lines
    Lines.push_back(NewLine);
    NewLine = nullptr;

    utils::log::debug("picked = %d, connect = %d", line.first, line.second);

    bool any_new = StateData.checkForNewBoxes(N, M);
    if (!any_new)
    {
        StateData.CurrentPlayer = !StateData.CurrentPlayer;
    }
    else
    {
        // keep drawing until no new box has been found
        mCPUDrawLine();
    }

    return error;
}
void BoardState::mGetHoveredDot()
{
    eng::draw::Dot *ptr = nullptr;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (Dots[i][j].Hovered)
            {
                ptr = &Dots[i][j];
                if (!PickedDot.second)
                {
                    PickedDot.first = i * M + j;
                    PickedDot.second = ptr;
                }
                else if (PickedDot.second && !ConnectDot.second)
                {
                    ConnectDot.first = i * M + j;
                    ConnectDot.second = ptr;
                }
                break;
            }
        }
    }
}
BoardState::~BoardState()
{
    for (auto &line : Lines)
    {
        delete line;
    }
    if (NewLine)
    {
        delete NewLine;
        NewLine = nullptr;
    }
}
} // namespace state
} // namespace gm