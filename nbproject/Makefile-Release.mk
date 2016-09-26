#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Config/ConfigHandler.o \
	${OBJECTDIR}/Game/Block.o \
	${OBJECTDIR}/Game/Board.o \
	${OBJECTDIR}/Game/BoardController.o \
	${OBJECTDIR}/Game/BoardEventHandler.o \
	${OBJECTDIR}/Game/EndlessGame.o \
	${OBJECTDIR}/Game/Game.o \
	${OBJECTDIR}/Game/GameEventHandler.o \
	${OBJECTDIR}/Game/GarbageBlock.o \
	${OBJECTDIR}/Game/KeyboardController.o \
	${OBJECTDIR}/Game/VsGame.o \
	${OBJECTDIR}/InputManager.o \
	${OBJECTDIR}/Menus/KeyConfigMenu.o \
	${OBJECTDIR}/Menus/MainMenu.o \
	${OBJECTDIR}/Menus/Menu.o \
	${OBJECTDIR}/Menus/MenuItem.o \
	${OBJECTDIR}/Menus/OptionsMenu.o \
	${OBJECTDIR}/Menus/PauseMenu.o \
	${OBJECTDIR}/Rendering/BoardRenderer.o \
	${OBJECTDIR}/Rendering/ChainPopup.o \
	${OBJECTDIR}/Rendering/ComboPopup.o \
	${OBJECTDIR}/Rendering/EndlessGameRenderer.o \
	${OBJECTDIR}/Rendering/GameRenderer.o \
	${OBJECTDIR}/Rendering/GbQueueRenderer.o \
	${OBJECTDIR}/Rendering/Particle.o \
	${OBJECTDIR}/Rendering/Popup.o \
	${OBJECTDIR}/Rendering/Renderer.o \
	${OBJECTDIR}/Rendering/TitleScreenRenderer.o \
	${OBJECTDIR}/Rendering/VsGameRenderer.o \
	${OBJECTDIR}/SDLContext.o \
	${OBJECTDIR}/States/EndlessGameState.o \
	${OBJECTDIR}/States/OptionsMenuState.o \
	${OBJECTDIR}/States/State.o \
	${OBJECTDIR}/States/StateManager.o \
	${OBJECTDIR}/States/TitleScreen.o \
	${OBJECTDIR}/States/VsGameState.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/panel-pop

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/panel-pop: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/panel-pop ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Config/ConfigHandler.o: Config/ConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/Config
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Config/ConfigHandler.o Config/ConfigHandler.cpp

${OBJECTDIR}/Game/Block.o: Game/Block.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/Block.o Game/Block.cpp

${OBJECTDIR}/Game/Board.o: Game/Board.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/Board.o Game/Board.cpp

${OBJECTDIR}/Game/BoardController.o: Game/BoardController.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/BoardController.o Game/BoardController.cpp

${OBJECTDIR}/Game/BoardEventHandler.o: Game/BoardEventHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/BoardEventHandler.o Game/BoardEventHandler.cpp

${OBJECTDIR}/Game/EndlessGame.o: Game/EndlessGame.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/EndlessGame.o Game/EndlessGame.cpp

${OBJECTDIR}/Game/Game.o: Game/Game.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/Game.o Game/Game.cpp

${OBJECTDIR}/Game/GameEventHandler.o: Game/GameEventHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/GameEventHandler.o Game/GameEventHandler.cpp

${OBJECTDIR}/Game/GarbageBlock.o: Game/GarbageBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/GarbageBlock.o Game/GarbageBlock.cpp

${OBJECTDIR}/Game/KeyboardController.o: Game/KeyboardController.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/KeyboardController.o Game/KeyboardController.cpp

${OBJECTDIR}/Game/VsGame.o: Game/VsGame.cpp 
	${MKDIR} -p ${OBJECTDIR}/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Game/VsGame.o Game/VsGame.cpp

${OBJECTDIR}/InputManager.o: InputManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InputManager.o InputManager.cpp

${OBJECTDIR}/Menus/KeyConfigMenu.o: Menus/KeyConfigMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/KeyConfigMenu.o Menus/KeyConfigMenu.cpp

${OBJECTDIR}/Menus/MainMenu.o: Menus/MainMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/MainMenu.o Menus/MainMenu.cpp

${OBJECTDIR}/Menus/Menu.o: Menus/Menu.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/Menu.o Menus/Menu.cpp

${OBJECTDIR}/Menus/MenuItem.o: Menus/MenuItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/MenuItem.o Menus/MenuItem.cpp

${OBJECTDIR}/Menus/OptionsMenu.o: Menus/OptionsMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/OptionsMenu.o Menus/OptionsMenu.cpp

${OBJECTDIR}/Menus/PauseMenu.o: Menus/PauseMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/Menus
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Menus/PauseMenu.o Menus/PauseMenu.cpp

${OBJECTDIR}/Rendering/BoardRenderer.o: Rendering/BoardRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/BoardRenderer.o Rendering/BoardRenderer.cpp

${OBJECTDIR}/Rendering/ChainPopup.o: Rendering/ChainPopup.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/ChainPopup.o Rendering/ChainPopup.cpp

${OBJECTDIR}/Rendering/ComboPopup.o: Rendering/ComboPopup.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/ComboPopup.o Rendering/ComboPopup.cpp

${OBJECTDIR}/Rendering/EndlessGameRenderer.o: Rendering/EndlessGameRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/EndlessGameRenderer.o Rendering/EndlessGameRenderer.cpp

${OBJECTDIR}/Rendering/GameRenderer.o: Rendering/GameRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/GameRenderer.o Rendering/GameRenderer.cpp

${OBJECTDIR}/Rendering/GbQueueRenderer.o: Rendering/GbQueueRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/GbQueueRenderer.o Rendering/GbQueueRenderer.cpp

${OBJECTDIR}/Rendering/Particle.o: Rendering/Particle.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/Particle.o Rendering/Particle.cpp

${OBJECTDIR}/Rendering/Popup.o: Rendering/Popup.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/Popup.o Rendering/Popup.cpp

${OBJECTDIR}/Rendering/Renderer.o: Rendering/Renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/Renderer.o Rendering/Renderer.cpp

${OBJECTDIR}/Rendering/TitleScreenRenderer.o: Rendering/TitleScreenRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/TitleScreenRenderer.o Rendering/TitleScreenRenderer.cpp

${OBJECTDIR}/Rendering/VsGameRenderer.o: Rendering/VsGameRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Rendering
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Rendering/VsGameRenderer.o Rendering/VsGameRenderer.cpp

${OBJECTDIR}/SDLContext.o: SDLContext.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SDLContext.o SDLContext.cpp

${OBJECTDIR}/States/EndlessGameState.o: States/EndlessGameState.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/EndlessGameState.o States/EndlessGameState.cpp

${OBJECTDIR}/States/OptionsMenuState.o: States/OptionsMenuState.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/OptionsMenuState.o States/OptionsMenuState.cpp

${OBJECTDIR}/States/State.o: States/State.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/State.o States/State.cpp

${OBJECTDIR}/States/StateManager.o: States/StateManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/StateManager.o States/StateManager.cpp

${OBJECTDIR}/States/TitleScreen.o: States/TitleScreen.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/TitleScreen.o States/TitleScreen.cpp

${OBJECTDIR}/States/VsGameState.o: States/VsGameState.cpp 
	${MKDIR} -p ${OBJECTDIR}/States
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/States/VsGameState.o States/VsGameState.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/panel-pop

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
