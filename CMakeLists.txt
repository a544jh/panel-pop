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
        Menus/KeyConfigMenu.cpp
        Menus/OptionsMenu.cpp
        Menus/MainMenu.cpp
        Menus/PauseMenu.cpp
        Menus/MenuItem.cpp
        Menus/Menu.cpp
        InputState.cpp InputState.h InputEvents/InputEvent.cpp InputEvents/InputEvent.h InputEvents/KeyboardKey.cpp InputEvents/KeyboardKey.h Config/InputConfig.cpp Config/InputConfig.h InputEvents/JoyButton.cpp InputEvents/JoyButton.h InputEvents/JoyHat.cpp InputEvents/JoyHat.h InputEvents/JoyAxisDirection.cpp InputEvents/JoyAxisDirection.h)

add_executable(panel-pop ${SRCS})

if (VITA)
	# Project start
	set(VITA_APPNAME PanelPop)
	set(VITA_TITLEID "PPOP00001")
	set(VITA_VERSION "00.03")

	# Find libraries
	include(FindPkgConfig)
	pkg_search_module(SDL2 REQUIRED sdl2)
	pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
	pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
	pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
	pkg_search_module(JSONCPP REQUIRED jsoncpp)

	target_link_libraries(${PROJECT_NAME} PRIVATE
		${SDL2_LIBRARIES}
		${SDL2_IMAGE_LIBRARIES}
		${SDL2_MIXER_LIBRARIES}
		${SDL2_TTF_LIBRARIES}
		${JSONCPP_LIBRARIES}
	)

	include_directories(
		${PROJECT_SOURCE_DIR}/include
		${SDL2_INCLUDE_DIRS}
		${SDL2_IMAGE_INCLUDE_DIRS}
		${SDL2_MIXER_INCLUDE_DIRS}
		${SDL2_TTF_INCLUDE_DIRS}
		${JSONCPP_INCLUDE_DIRS}
	)

	include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
	vita_create_self(panel-pop.self panel-pop)

	vita_create_vpk(panel-pop.vpk ${VITA_TITLEID} panel-pop.self
		    VERSION ${VITA_VERSION}
		    NAME ${VITA_APPNAME}
		    FILE platform/vita/sce_sys sce_sys
		    FILE platform/vita/panelpop.json panelpop.json
		    FILE assets/fonts assets/fonts
		    FILE assets/music assets/music
		    FILE assets/sfx assets/sfx
		    FILE assets/1p.png assets/1p.png
		    FILE assets/2p.png assets/2p.png
		    FILE assets/bg1.png assets/bg1.png
		    FILE assets/sprites.png assets/sprites.png
		    FILE assets/title.png assets/title.png
	)
ELSE()
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake")

	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)

	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(jsoncpp REQUIRED)
	get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)

	include_directories(${SDL2_INCLUDE_DIR}
		    ${SDL2_IMAGE_INCLUDE_DIR}
		    ${SDL2_TTF_INCLUDE_DIR}
		    ${SDL2_MIXER_INCLUDE_DIR}
		    ${JSON_INC_PATH})

	target_link_libraries(panel-pop ${SDL2_LIBRARY}
		    ${SDL2_IMAGE_LIBRARIES}
		    ${SDL2_TTF_LIBRARIES}
		    ${SDL2_MIXER_LIBRARIES}
		    jsoncpp)
ENDIF()
