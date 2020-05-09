########################################
# General setup
#

cmake_minimum_required(VERSION 3.5.0)

project(panel-pop)

# VitaSDK defines
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VITASDK})
                set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
        else()
                message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
        endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

# Project start
set(VITA_APPNAME panel-pop)
set(VITA_TITLEID "PPOP00001")
set(VITA_VERSION "01.00")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

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

set(VITA_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/")
set(SDL2_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/SDL2")
set(JSON_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/jsoncpp")
set(PNG_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/libpng16")
set(FLAC_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/FLAC")

include_directories(${INCLUDE_DIRECTORIES}
        ${VITA_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${JSON_INCLUDE_DIR}
        ${PNG_INCLUDE_DIR}
        ${FLAC_INCLUDE_DIR}
)

target_link_libraries(panel-pop
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        jsoncpp
        vita2d
        png16
        freetype
        vorbisfile
        vorbis
        ogg
        jpeg
        z
        m
        mikmod
        mpg123
        FLAC
        SceAppUtil_stub
        SceAudio_stub
        SceCommonDialog_stub
        SceCtrl_stub
        SceDisplay_stub
        SceGxm_stub
        SceHid_stub
        SceSysmodule_stub
        SceTouch_stub
)
vita_create_self(panel-pop.self panel-pop)

vita_create_vpk(panel-pop.vpk ${VITA_TITLEID} panel-pop.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APPNAME}
        FILE sce_sys/icon0.png sce_sys/icon0.png
        FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        FILE panelpop.json
        FILE assets/1p.png
        FILE assets/2p.png
        FILE assets/bg1.png
        FILE assets/sprites.png
        FILE assets/title.png
        FILE assets/fonts/square_sans_serif_7.ttf
        FILE assets/fonts/PressStart2P.ttf
        FILE assets/music/battle1_loop.ogg
        FILE assets/music/battle1_panic.ogg
        FILE assets/music/panelpop_intro.ogg
        FILE assets/music/panelpop_loop.ogg
        FILE assets/sfx/1x10.wav
        FILE assets/sfx/1x1.wav
        FILE assets/sfx/1x2.wav
        FILE assets/sfx/1x3.wav
        FILE assets/sfx/1x4.wav
        FILE assets/sfx/1x5.wav
        FILE assets/sfx/1x6.wav
        FILE assets/sfx/1x7.wav
        FILE assets/sfx/1x8.wav
        FILE assets/sfx/1x9.wav
        FILE assets/sfx/2x10.wav
        FILE assets/sfx/2x1.wav
        FILE assets/sfx/2x2.wav
        FILE assets/sfx/2x3.wav
        FILE assets/sfx/2x4.wav
        FILE assets/sfx/2x5.wav
        FILE assets/sfx/2x6.wav
        FILE assets/sfx/2x7.wav
        FILE assets/sfx/2x8.wav
        FILE assets/sfx/2x9.wav
        FILE assets/sfx/3x10.wav
        FILE assets/sfx/3x1.wav
        FILE assets/sfx/3x2.wav
        FILE assets/sfx/3x3.wav
        FILE assets/sfx/3x4.wav
        FILE assets/sfx/3x5.wav
        FILE assets/sfx/3x6.wav
        FILE assets/sfx/3x7.wav
        FILE assets/sfx/3x8.wav
        FILE assets/sfx/3x9.wav
        FILE assets/sfx/4x10.wav
        FILE assets/sfx/4x1.wav
        FILE assets/sfx/4x2.wav
        FILE assets/sfx/4x3.wav
        FILE assets/sfx/4x4.wav
        FILE assets/sfx/4x5.wav
        FILE assets/sfx/4x6.wav
        FILE assets/sfx/4x7.wav
        FILE assets/sfx/4x8.wav
        FILE assets/sfx/4x9.wav
        FILE assets/sfx/bigthump.wav
        FILE assets/sfx/chain.wav
        FILE assets/sfx/combo.wav
        FILE assets/sfx/countdown.wav
        FILE assets/sfx/cursor.wav
        FILE assets/sfx/fanfare1.wav
        FILE assets/sfx/fanfare2.wav
        FILE assets/sfx/fanfare3.wav
        FILE assets/sfx/go.wav
        FILE assets/sfx/pause.wav
        FILE assets/sfx/swap.wav
        FILE assets/sfx/thump.wav
)
