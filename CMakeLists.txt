########################################
# General setup
#

cmake_minimum_required(VERSION 3.5.0)

project(panel-pop)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SRCS
        Main.cpp
        Game/GameEventHandler.cpp
        Game/Block.cpp
        Game/BoardController.cpp
        Game/Game.cpp
        Game/GarbageBlock.cpp
        Game/KeyboardController.cpp
        Game/Board.cpp
        Game/VsGame.cpp
        Game/EndlessGame.cpp
        Game/BoardEventHandler.cpp
        Config/ConfigHandler.cpp
        SDLContext.cpp
        Rendering/Popup.cpp
        Rendering/EndlessGameRenderer.cpp
        Rendering/ChainPopup.cpp
        Rendering/GameRenderer.cpp
        Rendering/TitleScreenRenderer.cpp
        Rendering/BoardRenderer.cpp
        Rendering/Particle.cpp
        Rendering/ComboPopup.cpp
        Rendering/GbQueueRenderer.cpp
        Rendering/VsGameRenderer.cpp
        Rendering/Renderer.cpp
        States/TitleScreen.cpp
        States/OptionsMenuState.cpp
        States/DemoGameState.cpp
        States/StateManager.cpp
        States/VsGameState.cpp
        States/AIGameState.cpp
        States/State.cpp
        States/EndlessGameState.cpp
        States/GameState.cpp
        States/FadeTransition.cpp
        AI/BoardScanner.cpp
        AI/AIBoardController.cpp
        InputManager.cpp
        Util/Paths.cpp
        Menus/KeyConfigMenu.cpp
        Menus/OptionsMenu.cpp
        Menus/MainMenu.cpp
        Menus/PauseMenu.cpp
        Menus/MenuItem.cpp
        Menus/Menu.cpp
        InputState.cpp InputState.h InputEvents/InputEvent.cpp InputEvents/InputEvent.h InputEvents/KeyboardKey.cpp InputEvents/KeyboardKey.h Config/InputConfig.cpp Config/InputConfig.h InputEvents/JoyButton.cpp InputEvents/JoyButton.h InputEvents/JoyHat.cpp InputEvents/JoyHat.h InputEvents/JoyAxisDirection.cpp InputEvents/JoyAxisDirection.h
        Debug/DebugOptions.cpp Debug/DebugOptions.h Debug/DebugInput.cpp Debug/DebugInput.h Debug/StateDump.cpp Debug/StateDump.h Debug/DebugConsole.cpp Debug/DebugConsole.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake")

add_executable(panel-pop ${SRCS})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
# Use Boost's own BoostConfig.cmake (Boost >= 1.70) rather than the FindBoost
# module, which newer CMake versions have deprecated.
if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()
find_package(Boost REQUIRED) # header-only use (property_tree)

# The sources include SDL headers both as <SDL_foo.h> and <SDL2/SDL_foo.h>, so
# besides the .../include/SDL2 dirs found above we also need their parent dirs.
# On Linux and Intel Macs the parent (/usr/include, /usr/local/include) is a
# default compiler search path, but e.g. /opt/homebrew/include is not.
set(SDL2_ALL_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR})
foreach (dir ${SDL2_ALL_INCLUDE_DIRS})
    get_filename_component(dir_name ${dir} NAME)
    if (dir_name STREQUAL "SDL2")
        get_filename_component(parent_dir ${dir} DIRECTORY)
        list(APPEND SDL2_ALL_INCLUDE_DIRS ${parent_dir})
    endif ()
endforeach ()
list(REMOVE_DUPLICATES SDL2_ALL_INCLUDE_DIRS)

include_directories(${SDL2_ALL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

target_link_libraries(panel-pop ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES})

########################################
# Installation
#
# PANELPOP_DATADIR is baked into the binary so it can find its bundled assets
# after a system install. When run from the source tree (a ./assets directory
# is present in the working dir) that location takes precedence instead.

include(GNUInstallDirs)

set(PANELPOP_DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/panel-pop"
        CACHE PATH "Directory where panel-pop's assets are installed")

target_compile_definitions(panel-pop PRIVATE "PANELPOP_DATADIR=\"${PANELPOP_DATADIR}\"")

install(TARGETS panel-pop RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install the runtime assets, skipping editable source art (GIMP/FamiTracker).
install(DIRECTORY assets DESTINATION ${PANELPOP_DATADIR}
        PATTERN "*.xcf" EXCLUDE
        PATTERN "*.ftm" EXCLUDE
        PATTERN "*.ico" EXCLUDE)

# Desktop entry and icon for menu integration.
install(FILES panel-pop.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES assets/icon.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps
        RENAME panel-pop.png)
