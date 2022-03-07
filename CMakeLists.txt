cmake_minimum_required(VERSION 3.0.0)
project("dots-and-boxes" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "src"
)

find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

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
    "src/game/state/pvp_state.cpp"
    "src/game/state/pvc_state.cpp"

    # engine

    # drawables
    "src/engine/drawable/box.cpp"
    "src/engine/drawable/dot.cpp"
    "src/engine/drawable/line.cpp"
    "src/engine/drawable/character.cpp"
    "src/engine/drawable/text.cpp"
)

target_link_libraries(
    ${PROJECT_NAME}

    PRIVATE GLEW::GLEW
    PRIVATE SDL2::SDL2
    PRIVATE glm::glm
    PRIVATE imgui::imgui
    PRIVATE freetype
)