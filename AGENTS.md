# AGENTS.md — notes for automated/assisted work on panel-pop

Practical notes for building, running and debugging this repo, including a
headless debug mode that lets you inspect game state and inject input without a
display. Commands below were verified on Ubuntu 26.04 / arm64 / SDL 2.32.

## What this is

C++11 clone of Panel de Pon (Tetris Attack) on SDL2. ~7k LOC, single CMake
target `panel-pop`, no test framework, no CI.

| Path | Contents |
|---|---|
| `Main.cpp` | entry point: parses debug options, inits SDL, runs `StateManager` |
| `SDLContext.cpp` | window/renderer/fonts/sprite sheet/audio loading |
| `States/` | screen state machine: `TitleScreen`, `OptionsMenuState`, `EndlessGameState`, `VsGameState`, `AIGameState`, `DemoGameState`, `FadeTransition` |
| `StateManager` | main loop: 16 ms logic tick, render throttled to ~200 fps |
| `Game/` | rules: `Board`, `Block`, `GarbageBlock`, `EndlessGame`, `VsGame`, controllers |
| `Rendering/` | per-state renderers, popups, particles |
| `Menus/` | main/pause/options/key-config menus |
| `AI/` | `BoardScanner` + `AIBoardController` (plays board 2 in "VS AI") |
| `Config/`, `InputEvents/` | `panelpop.ini` handling, key/joystick bindings (`KUp`, `J0_B1`) |
| `Debug/` | headless debug mode (this file's subject) |
| `assets/` | sprites, fonts, .ogg music, .wav sfx — required at runtime |

## Build

```
sudo apt-get install -y cmake pkg-config libboost-dev libsdl2-dev \
  libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev gdb valgrind xvfb
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

Notes:
- Boost is header-only here (`boost::property_tree` in `Config/ConfigHandler`),
  `libboost-dev` is enough; no boost libraries are linked.
- `CMakeLists.txt` asks for `cmake_minimum_required(VERSION 3.5.0)`: CMake 4.x
  configures it with a deprecation warning, but a future CMake will reject it.
  Bump to `3.10` if it starts failing.
- `CMake/FindSDL2*.cmake` are vendored (older, pre-`SDL2::SDL2` upstream
  configs). They find Ubuntu's packages fine.

## Running headless (no display needed)

```
./build/panel-pop --headless --frames 300     # 300 logic ticks, then exit
./build/panel-pop --help                      # full option list
```

| Option | Effect | env equivalent |
|---|---|---|
| `--headless` | SDL dummy video+audio drivers; no window, no sound device | `PANELPOP_HEADLESS=1` |
| `--debug-stdin` | debug console on stdin, one tick at a time (implies `--headless`) | `PANELPOP_DEBUG_STDIN=1` |
| `--frames N` or `--frames=N` | stop after N *logic* ticks (0 = run until quit); in the console it caps the total ticks stepped across all commands | `PANELPOP_MAX_TICKS=N` |
| `--no-render` | skip `State::render()`; faster, but no render coverage | `PANELPOP_RENDER=0` |

In normal (non-console) mode a logic tick is 16 ms of wall clock, so
`--frames 300` ≈ 4.8 s of gameplay.

With `--headless`, `SDL_CreateRenderer(…, SDL_RENDERER_ACCELERATED)` fails by
design and `SDLContext` falls back to `SDL_RENDERER_SOFTWARE`, so rendering and
screenshots still work — that is the recommended way to catch render regressions
headless.

To run the *normal* (non-dummy) code path on a machine without a screen:

```
xvfb-run -a -s "-screen 0 800x600x24" ./build/panel-pop --frames 100
```

(You will see `ALSA lib … underrun occurred` noise: harmless, nothing is
consuming the PCM device.)

## Debug console (`--debug-stdin`)

The console replaces the main loop: nothing advances until you tell it to, so a
session is reproducible. Pipe commands in for scripting, or use it interactively.

```
printf 'goto endless\nwait 1100\ntick\nwait 1100\ntick\nwait 1100\ntick\nwait 1100\ntick 100\nboard\npress left\ntick\njson\nquit\n' \
  | ./build/panel-pop --debug-stdin --no-render
```

(The repeated `wait`/`tick` pairs just clear the wall-clock countdown — see
Gotchas below.)

| Command | Effect |
|---|---|
| `state`, `s` | current state, game/board summary (score, cursor, chain, stack, garbage, points) |
| `board [1\|2]`, `b` | ASCII dump of one board (row 11 = top visible, row 0 = bottom) |
| `json` | one-line machine-readable snapshot of everything (grids, cursors, scores, queues) |
| `menu` | menu items with the current selection marked |
| `press <key>` | queue a one-tick key press (applied on the **next** tick) |
| `send <key> [n]` | `press <key>` then advance n ticks (default 1) |
| `down <key>` / `up <key>` | hold / release a key across ticks |
| `tick [n]`, `t [n]` | advance n logic ticks (renders too unless `--no-render`) |
| `run [n]` | `tick n` then print a summary |
| `wait <ms>` | real-time sleep, needed for `SDL_GetTicks()` timers |
| `goto <s>` | switch state directly: `title\|endless\|vs\|ai\|demo\|options` |
| `shot <file.bmp>` | save the rendered frame as a 640×480 BMP |
| `echo on\|off` | echo piped commands (on by default when stdin is not a tty) |
| `quit`, `q` | leave (prints the tick count) |

Key names: `up down left right x z esc return enter space tab ctrl alt shift
pause`, single characters (`a`, `1`), SDL scancode names (`RIGHT`,
`SDL_SCANCODE_RIGHT`), and the ini binding format (`KUp`).

Default bindings for reference: arrows move the cursor, `x` accepts/swaps, `z`
cancels/raises the stack, `Esc`/`Return` pause/start. Menus react to `x`
(accept), `z` (cancel) and the arrows — **not** to `Return`.

### Board dump legend

`.` empty, `P Y R C G` colors (purple/yellow/red/cyan/green), lowercase
`p y r c g` = part of a chain, `*` matched/exploding, `#` garbage, `g` gray
garbage. Row indices: rows above `TOP_ROW` (11) are off-screen reserve
(`BOARD_HEIGHT` is 24, only 0–11 are drawn).

### How input injection works

`SDL_GetKeyboardState()` returns a read-only array, so injected keys cannot be
written into it. Instead:

- `Debug/DebugInput` keeps injected key state (a one-tick press set, plus held
  keys), applied each tick.
- `InputEvents/KeyboardKey::isActive()` (the binding layer every player action
  goes through) ORs that state with SDL's real array.
- `InputManager::poll()` merges it into a private copy that `_keys`/`_prevKeys`
  compare against, so the shortcut paths (`Alt+Return`, `Ctrl+F`,
  `anyKeyDown()`) see it too.
- A real `SDL_KEYDOWN`/`SDL_KEYUP` pair is also pushed into the SDL queue, so
  the key-rebinding menu (which reads events, not key state) can be driven.

Injection is inert unless `--debug-stdin` is active; a normal run is unchanged.
A press is live for exactly one tick, which is why `press` must be followed by
`tick` (or use `send`). Joystick/gamepad injection is not implemented.

## Gotchas

- **Wall-clock timers.** Logic ticks are deterministic in the console, but many
  timers use `SDL_GetTicks()`: the game countdown (`COUNTDOWN_STEP_MS = 1000`,
  3 steps per game), pause timing, menu blinking, `Game::getTime()`. Stepping
  4000 ticks in a script takes almost no real time, so a game stays in
  `COUNTDOWN` and scores nothing until you `wait ~1100` **per countdown step**
  (once per second of real time). A soak that never scores is usually this.
- **`panelpop.ini` is a runtime file.** `Paths::configFile()` prefers
  `./panelpop.ini`, so running from the source tree rewrites the checked-in
  copy (new high scores, option changes) and dirties git. Copy it aside first,
  or `git checkout panelpop.ini` after a session.
- **Asset lookup.** `Paths::asset()` uses `./assets` from the working directory
  if present, otherwise the compiled-in `PANELPOP_DATADIR`
  (`/usr/share/panel-pop` when installed; `.` for a plain in-tree build). Run
  from the repo root, or install.
- **SDL init is strict now.** `SDLContext::init()` returns false on a
  video/renderer/font/asset failure and `Main.cpp` exits 1. Previously failures
  were printed and ignored, producing a black window and null textures.
  `Mix_OpenAudio` failure is still non-fatal (the game runs silent).
- **Title screen auto-demo.** 600 idle ticks (`DEMO_TIMEOUT`) start
  `DemoGameState`; a long scripted session on the title screen ends up there.
  The switch goes through `FadeTransition`, which is tick-based
  (`TRANSITION_TICKS = 120`), so it needs 120 more ticks before
  `state` reports `DemoGameState`. `goto` skips fades.
- **Game over needs `Esc`/`Return`** to reset the game (see
  `GameState::tick()`).

## Debugging recipes

```
# deterministic session under valgrind (clean as of this writing)
printf 'goto endless\nwait 1100\ntick 200\npress x\ntick 200\nquit\n' \
  | valgrind --leak-check=full --error-exitcode=9 ./build/panel-pop --debug-stdin --no-render

# random-input soak for crashes: drive the console from a script, see below
# render check: same but with rendering on, plus screenshots
printf 'goto endless\nwait 1100\ntick\nwait 1100\ntick\nwait 1100\ntick\nwait 1100\ntick 120\nshot /tmp/f.bmp\nquit\n' \
  | ./build/panel-pop --debug-stdin

# gdb on a scripted session
gdb -q -ex 'break StateManager::step' -ex run --args ./build/panel-pop --debug-stdin
```

For sanitizers, configure a separate dir:

```
cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"
cmake --build build-asan -j
```

Minimal Python driver for scripted sessions (no dependencies; adjust `CMD`):

```python
import json, subprocess
CMD = ["./build/panel-pop", "--debug-stdin", "--no-render"]
p = subprocess.Popen(CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                     text=True, bufsize=1)
def send(c): p.stdin.write(c + "\n"); p.stdin.flush()
def snap():                                  # read one json snapshot
    send("json")
    while True:
        line = p.stdout.readline().strip()
        if line.startswith("{"): return json.loads(line)
send("goto endless")
p.stdout.readline()
for _ in range(4): send("wait 1100"); send("tick")   # clear the countdown
send("press x"); send("tick 30")
b = snap()["boards"][0]
print(b["score"], b["boardState"], b["cursorX"], b["cursorY"])
send("quit"); p.wait(timeout=10)
```

A quick end-to-end check that everything works: a 4000-tick random-input endless
session should reach a few hundred points, print `board=RUNNING`, then
`game=ENDED board=GAME_OVER` once the stack tops out — i.e. input, matching,
chains, stack rise and game over all visible through the console.

## Known issues found with the debug console (not yet fixed)

UBSan/ASan trips immediately on a scripted session; these are pre-existing game
bugs, not debug-mode bugs:

- `Game/Board.cpp:84` in `Board::fillBufferRow()` —
  `colors.remove(_bufferRow[i - 1].b._color)` with `i == 0` reads
  `_bufferRow[-1]` (UBSan `index -1 out of bounds for type 'Tile [6]'`, ASan
  stack-buffer-underflow). Runs on every `Board` construction (`Board.cpp:34`)
  and on each stack advance (`Board.cpp:704`).
- `Game/GarbageBlock.cpp:35` in `GarbageBlock::fillBufferRow()` — same
  `_bufferRow[i - 1]` pattern at `i == 0`; hit whenever garbage spawns
  (VS / AI games). This one is an actual ASan crash.
- `Board::fillRandom()` (around `Board.cpp:66`) shows the intended pattern:
  both neighbour lookups are guarded with `if (j - 1 >= 0)` /
  `if (i - 1 >= 0)`. The two `fillBufferRow()`s just lack that guard, so the
  fix is the same guard (the value is only used to exclude a colour from the
  random pick).

Until they are fixed, expect ASan runs of VS/AI games to abort; endless mode is
usable under ASan apart from the construction-time report.
