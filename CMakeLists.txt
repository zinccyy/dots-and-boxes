cmake_minimum_required(VERSION 3.0.0)
project("dots-and-boxes" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if(EMSCRIPTEN)
add_compile_options("-s" "USE_FREETYPE=1")
endif()

add_executable(
    ${PROJECT_NAME}

    "src/main.cpp"
    "src/game/game.cpp"
    "src/utils/log.cpp"

    # utils/fs
    "src/utils/fs/fs.cpp"

    # utils/gl
    "src/utils/gl/shader_program.cpp"
    "src/utils/gl/gl.cpp"

    # game/state
    "src/game/state/main_menu.cpp"
    "src/game/state/game_state.cpp"
    "src/game/state/board_state.cpp"

    # engine
    "src/engine/timer.cpp"

    # drawables
    "src/engine/drawable/box.cpp"
    "src/engine/drawable/dot.cpp"
    "src/engine/drawable/line.cpp"
    "src/engine/drawable/character.cpp"
    "src/engine/drawable/text.cpp"
)

if (EMSCRIPTEN)
    include_directories(
        "src"
        "deps/imgui"
        "deps/imgui/backends"
        "deps/glm"
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ../assets -s USE_SDL=2 -s FULL_ES3 -s USE_FREETYPE=1 -s DISABLE_EXCEPTION_CATCHING=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    target_sources(
        ${PROJECT_NAME}

        PRIVATE

        ../deps/imgui/imgui.cpp 
        ../deps/imgui/imgui_demo.cpp 
        ../deps/imgui/imgui_draw.cpp 
        ../deps/imgui/imgui_tables.cpp
        ../deps/imgui/imgui_widgets.cpp
        ../deps/imgui/backends/imgui_impl_sdl.cpp
        ../deps/imgui/backends/imgui_impl_opengl3.cpp
    )
else()
    include_directories(
        "src"
    )
    find_package(glm CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(freetype CONFIG REQUIRED)

    target_link_libraries(
        ${PROJECT_NAME}

        PRIVATE GLEW::GLEW
        PRIVATE SDL2::SDL2
        PRIVATE glm::glm
        PRIVATE imgui::imgui
        PRIVATE freetype
    )
endif ()