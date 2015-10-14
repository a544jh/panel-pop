bin_PROGRAMS = panel-pop
panel_pop_SOURCES = InputManager.cpp \
    main.cpp \
    Rendering/ScorePanel.cpp \
    Rendering/ChainPanel.cpp \
    Rendering/GameRenderer.cpp \
    Rendering/Renderer.cpp \
    Rendering/BoardRenderer.cpp \
    Game/Game.cpp \
    Game/GarbageBlock.cpp \
    Game/Board.cpp \
    Game/Block.cpp \
    SDLContext.cpp

panel_pop_CPPFLAGS = $(SDL2_CFLAGS) -std=c++11
panel_pop_LDADD = $(SDL2_LIBS)
