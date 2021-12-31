#include <game/game.hpp>
#include <utils/log.hpp>

// SDL2
#undef main

int main()
{
    int error = 0;
    gm::Game game;

    error = game.init();
    if (error)
    {
        goto error_out;
    }

    error = game.initOpenGLData();
    if (error)
    {
        goto error_out;
    }

    goto out;

error_out:
    utils::log::error("unable to start the dots-and-boxes game... aborting");
    return -1;
out:
    utils::log::info("starting dots-and-boxes game");
    return game.run();
}
