#include <game/game.hpp>
#include <utils/log.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// SDL2
#undef main

// emscripten main loop
#ifdef __EMSCRIPTEN__
void emscripten_loop(void *arg)
{
    gm::Game *game = (gm::Game *)arg;

    // game loop
    game->loopIter();

    // return is the game still running
    // return game->getRunningState();
}
#endif

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

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscripten_loop, &game, 0, true);
#else
    while (game.getRunningState())
    {
        game.loopIter();
    }
#endif
    return 0;
}
