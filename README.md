# panel-pop
A Panel de Pon / Tetris Attack / Puzzle League clone made with C++ and SDL

#### [Download latest release](https://github.com/a544jh/panel-pop/releases/latest)

[Gameplay video](http://www.youtube.com/watch?v=z_DVUP2aS9Y)

![screenshot](screenshot.png)

### Features:
* Original-like mechanincs modeled after SNES version
* Additional visual gameplay tweaks
* Single player endless mode
* Simple AI
* Two-player VS game with garbage blocks


### Build:
* Linux
1. `cmake`
2. `make`

* MacOS
1. `cmake`
2. `make`

* Windows

(you'll need MinGW in the root folder)
1. `cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"`
2. `mingw32-make.exe`


### Controls:

Default gameplay keys:
    * Arrow keys - move cursor
    * `X` - accept/swap blocks
    * `Z` - cancel/raise stack
    * `5` - pause/start

Debug keys:
    * `Esc` - reset boards
    * `1`,`2`,`3`,`4`,`6`,`7`,`8` - drop garbage block

Misc:
    * `Ctrl`+`F` - toggle fps diplay
    * `Alt`+`Return` - toggle fullscreen
