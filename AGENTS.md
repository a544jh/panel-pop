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
sudo apt-get update
sudo apt-get install -y cmake pkg-config libboost-dev libsdl2-dev \
  libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev gdb valgrind xvfb
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

Notes:
- Don't skip `apt-get update` in a fresh container or CI image: they often ship
  with empty package lists, and the install then fails with `Unable to locate
  package libboost-dev` (and the same for every SDL package).
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
| `--seed N` | seed the game RNG (`std::srand`) so board layouts repeat; also honoured by the debug console | `PANELPOP_SEED=N` |

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

This uses the real audio driver. On a machine with a sound card you will see
`ALSA lib … underrun occurred` noise: harmless, nothing is consuming the PCM
device. On one with no sound device at all (a container or CI box without
`/dev/snd`), you get `ALSA lib … Unknown PCM default` and then
`Mix_OpenAudio: ALSA: Couldn't open audio device: …, running without sound`,
and the game runs silent (verified on Ubuntu 24.04 / x86_64 / SDL 2.30). To
skip the ALSA noise, add `SDL_AUDIODRIVER=dummy` in front of the command.

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
| `strategy [key] [board]` | print/swap the AI strategy of the running game (board 2 by default): `basic`, `qwen`, `qwen,k=v,...`; a human board is replaced by an AI |
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

## AI strategies (`AI/`)

Board 2 of a "VS AI" game is driven by a strategy object; every other board is
played by a human.

| File | Role |
|---|---|
| `AIStrategy.h/.cpp` | interface, shared cursor-path helper, registry, `parseSpec()` |
| `QwenStrat.h/.cpp` | the default strategy, key `qwen` |
| `ClaudeStrategy.h/.cpp` | plays like a good human player, key `claude` (see "Claude" below) |
| `BasicStrategy.h/.cpp` | the original AI, key `basic` (`BoardScanner` + the old `AIBoardController` move ordering) |
| `SimBoard.h/.cpp` | headless copy of the board rules used for lookahead by `qwen` |
| `ClaudeSim.h/.cpp` | tick-exact board model used by `claude`, checked by `claudesim-check` |
| `AIBoardController.*`, `BoardScanner.*` | board 2 controller / the chain scorer the original AI wraps |

The controller outlives the board: `VsGame::reset()` (the next round of a VS
match, after `Esc`/`Return` on a game-over screen) assigns the `Board::reset()`
copy back over the same board object, so the `Board&` in `BoardController` stays
valid but `Board::getTicksRun()` starts from 0 again. `AIBoardController::tick()`
notices that counter going backwards and calls `AIStrategy::reset()` (clears
`QwenStrat::_nextPlanTick`) plus drops the actions queued for the old board.
Strategies throttle themselves on board ticks, so skipping that call leaves the
AI silently idle for as long as the previous round lasted.

Selection, in order of precedence:

1. debug console `strategy <key> [board]` on a running game - live swap, and
   it replaces a human controller with an AI one, which is how the benchmarks
   drive both boards (`GameState::setAiStrategy`)
2. the `[ai]` section of `panelpop.ini`, written by the options menu:
   - `strategy=` ("AI strategy", `ConfigHandler::getAiStrategy()`): the "VS
     AI" opponent (board 2 of an `AIGameState`) and player 1 of the title
     screen demo (`DemoGameState`);
   - `demo_p2_strategy=` ("P2 AI", `ConfigHandler::getDemoP2Strategy()`):
     player 2 of the demo. Until it is set it follows `strategy=`, so by
     default the demo shows the selected AI against itself.

   Both are read when the state starts, so a change applies to the next game
   or demo.
3. the first entry of `DESCRIPTORS` in `AIStrategy.cpp` (currently `qwen`)
   when the ini has nothing

On the console, `strategy` with no argument prints board 2's strategy, and
`strategy <key>` *sets* it: `strategy 1` asks for a strategy called "1",
which falls back to the default.

There is no strategy env var. The environment only tunes the planners:
`PANELPOP_QWEN_WEIGHTS="k=v,..."` and `PANELPOP_AI_DEBUG=1` (prints each plan)
for `qwen`, `PANELPOP_CLAUDE_OPTIONS="k=v,..."`, `PANELPOP_CLAUDE_DEBUG=1` and
`PANELPOP_CLAUDE_VERIFY=1` for `claude`. A key may also carry tuning inline:
`qwen,cand=3,budget=60`, `claude,pace=3` (`AIStrategies::parseSpec`).

### QwenStrat

One *plan* at a time: it searches, queues the actions of the best move, and
searches again when the queue is empty - after `replan` logic ticks if the
board is at rest, `replanActive` ticks while tiles are still moving.

The search (`plan()` -> `generateMoves()` -> `playAndValue()`):

1. Candidates: every legal adjacent swap, plus *pass*, plus a forced stack
   raise.
2. Each candidate is played out on a `SimBoard` copy for `ticks` logic ticks
   (swap, gravity, chains, buffer-row refill, stack rise) and judged by
   `evaluate()`; a board that tops out scores `death`.
3. The best `cand` candidates get a second ply: their horizon position is
   played `followup` more ticks with its best follow-up swap, discounted by
   `followupWeight`. This is what makes the AI build chains instead of only
   clearing what is directly matchable (`followup=0` turns it off).
4. The search is *anytime*: it keeps adding second-ply lookahead until `budget`
   **milliseconds of wall clock** are used. On a loaded machine (or with a
   high `--jobs` in the benchmarks) the AI therefore looks less deep - pin
   `budget` and keep the machine otherwise idle when comparability matters.
5. Cursor travel is subtracted from a move's value (`travel` per logic tick of
   walking, `appendCursorPath()` for the actions), so equally good moves are
   chosen by how fast they can be played.

`evaluate()` scores the *delta* against the position at the search root:

| Knob | Default | Counts |
|---|---|---|
| `score` | 1.0 | points gained - the model scores with the same `chainScoring()`/`comboScoring()` tables as `Board` |
| `garbage` | 45 | garbage rows sent to the opponent |
| `chainLevel` | 25 | per chain level reached, and per extra level of a chain still running at the horizon |
| `pendingScore` | 12 | blocks already exploding/settling when the horizon ends |
| `height`, `heightSquare` | 6, 0.45 | column heights, linear and quadratic |
| `holes` | 16 | empty cells under the top of a column |
| `garbageRows` | 14 | garbage still on the board |
| `panicBlocks` | 30 | blocks at or above `PANIC_HEIGHT` |
| `topThreat` | 220 | stack within 3 rows of `TOP_ROW`, and the grace timer running - the real killer |
| `matchOptions`, `optionBlocks` | 40, 4 | swaps that score right away (from `immediateMatches()`) |
| `adjacency` | 3 | same-colour neighbour pairs, i.e. one move from a match |
| `columnSpread` | 4 | difference between the tallest and shortest column |
| `raisePenalty` | 0 | raising the stack (only used by the survival mode below) |
| `death` | -40000 | the simulated board topped out |

When the board is in danger (`survival`) the height terms are weighted 1.5x
harder and garbage sent is ignored - digging out beats poking the opponent.

Tuning knobs: the search shape is `ticks`, `followup`, `cand`, `budget`,
`replan`, `replanActive`, `followupWeight`, `travel`; the evaluation weights are
the names in the table above. Set them as `qwen,cand=3,budget=60`, as
`PANELPOP_QWEN_WEIGHTS="holes=30,..."`, or on the console
(`strategy qwen,height=12 2`); `plan()` re-reads the env each time, so the
console path honours them too.

### SimBoard

`SimBoard` re-implements `Board` without SDL. `tick()` mirrors `Board::tick()`
(swap animation and the `SWAP_DELAY` exchange, gravity/collapse, matches,
chains and their score, floating/explosion timers, the stack rise, garbage
spawns including the gray/transformation rows), and the input layer mirrors
`Board::inputSwapBlocks()` / `Board::inputForceStackRaise()` (`canSwap`,
`startSwap`, `forceRaise`). `import(const Board&)` copies a live board into the
model; `fillBufferRow()` reproduces the buffer-row pattern of the real game (a
colour different from the block below and from the buffer neighbour to the
left); `setRng(seed)` gives the model its own deterministic PRNG for the
colours it has to invent, reset before every candidate so the candidates stay
comparable.

Two consequences worth knowing:

- Keep the two in sync by hand when you change `Board` - the buffer row and the
  gravity rules in particular. A model that collapses or refills differently
  from the real board invalidates the search (that is what made the AI play
  badly once: the model invented buffer rows the real board never produces).
- Nothing writes back into a live board: `plan()` only reads.

`SimBoard` has known divergences from `Board` (left as they are; `claude` uses
its own model, below): `import()` drops the float timer of floating blocks
(they start falling up to 12 ticks early), the empty half of a block-into-air
swap is turned into a `BLOCK` of colour `COUNT` that stays on the board after
the swap, the empty half's swap timer is not imported, and garbage `falling`
is always imported as false. The lockstep checker in `tools/claudesim-check`
found them; pointing it at `SimBoard` reproduces them.

### Claude (`ClaudeStrategy`, key `claude`)

Built to *look* like a good Panel de Pon player: it builds chains on purpose,
extends running chains with skill-chain swaps timed against the falling
blocks, keeps a flat stack at a useful height (raising it when material runs
low), digs through garbage, and moves at a human pace instead of one input
per frame. Selectable in Options -> "AI strategy" (menu order: QwenStrat,
Claude, Basic) or with `strategy claude [board]` on the console.

One decision (`plan()`):

1. Import the board into `ClaudeSim` (exact, see below) and list candidate
   moves: every single swap, every *drag* (one block slid 2-3 columns along
   its row, swap after swap, `drag`), a forced raise, and waiting. Each move
   is the literal action list the controller will play: cursor path first,
   one input every `pace` ticks (`fastPace` while a chain runs, in danger or
   with garbage queued), the planning tick itself included.
2. Play every candidate on a copy of the model with exactly those actions,
   then let the model run on until nothing is active and any chain has ended
   (`rollout()`, capped at 1500 ticks). Timing is therefore real: a swap that
   would extend a chain is only found if the cursor gets there before the
   falling block lands - that is where the skill chains come from.
3. Value = what happened + what the settled board promises (`evaluate()`):
   - sent: `row * rows * (1 + chainGrowth * (rows - 1))` per finished chain
     (rows = chain - 1, as `VsGame` sends), `comboCell` per garbage cell a
     combo sends, `clearCell` per garbage cell turned into blocks;
   - chain potential: the best chain one more swap or short drag could start
     on the settled board, found by a timing-free resolver (apply the move,
     let blocks fall, clear runs of 3+, repeat; every round after the first
     is a link), times `potential`; plus `options` per available match;
   - stack: `danger * (h - 7)^2` per column above 7 rows (garbage queued to
     drop counts as height), `flat` per unit of height difference, `tower`
     for lone tall columns, `garbage` per garbage cell, a big penalty while
     the top-out grace timer runs, and `low` per row of *blocks* missing
     below `comfort` rows. Garbage does not count as material - a board that
     is mostly garbage over a few blocks is the most dangerous one there is.
   - minus `effort` per input, so nothing is done for nothing.
4. The best `width` moves on a calm board get a second move searched on top
   (`depth=2`), weighted `follow`: setups that pay off one move later.
   During a chain the single-move search is enough, and cheaper.
5. Hysteresis: swapping the pair just swapped straight back costs `undo`
   unless it makes something happen; standing still while about to top out
   or buried costs `scramble`, so it fights on like a player would.

Knobs (`claude,k=v,...` or `PANELPOP_CLAUDE_OPTIONS`): `pace` (4), `fastPace`
(3), `drag` (3), `depth` (2), `width` (4), `budget` (60000 simulated ticks per
plan, deterministic - not wall clock), `waitCalm`/`waitActive` (6/3 ticks
between looks when waiting), and the weights `row` 100, `chainGrowth` 0.25,
`potential` 0.6, `comboCell` 10, `clearCell` 8, `effort` 1.5, `flat` 4, `tower`
15, `low` 20, `comfort` 6, `danger` 25, `garbage` 2, `options` 3, `follow` 0.8,
`undo` 40, `scramble` 80.

- `PANELPOP_CLAUDE_DEBUG=1` prints one line per plan (`claude: t=... chain=...
  swap(x,y) v=... sent=... maxc=... pot=... sims=... ms=... [inputs]`) and
  `claude: skill+N at chain C` whenever a move makes a running chain N links
  longer than just watching it would.
- `PANELPOP_CLAUDE_VERIFY=1` (also on with DEBUG) checks at every plan that the
  real board is exactly where the model said it would be (cells, cursor,
  stack, score) and prints `prediction miss` otherwise. Garbage the opponent
  sent in between and invented colours are not counted as misses. It reported
  none over thousands of plans in VS games.
- Cost: `AI/ClaudeSim.cpp` and `AI/ClaudeStrategy.cpp` are compiled with
  `-O2` even in Debug builds (`set_source_files_properties` in
  `CMakeLists.txt`).
  Measured in a VS game: a plan every ~6.5 ticks, 3 ms on average (median 2,
  95th percentile 10, worst 26), about 0.5 ms per game tick overall.
- Pace: 4 ticks per input calm, 3 in a hurry - 15-20 inputs/s, fast but
  human. `pace=3,fastPace=2` plays stronger (see benchmarks) and looks
  robotic sooner; `pace=1` is frame-perfect.

### ClaudeSim

`ClaudeSim` is `claude`'s board model: a compact, copyable re-implementation of
`Board::tick()` and the board input functions, plus the cursor, so an action
list plays on it exactly as `AIBoardController` plays it on the real board
(`step(action)` = one controller tick + one board tick). It follows
`Board.cpp` function by function, quirks included: the
`it = list.erase(it); continue;` loops in `handleGarbageQueue()` and
`handleBlockTimers()` skip the next element and, when the erased one was last,
wrap round to `begin()` (libstdc++) and visit the rest a second time; combos
score the chain bonus twice; the panic flag only looks at row 9; `+10` per
block is scored at the explosion *animation* tick; garbage only spawns when
rows 10-23 are completely empty; and so on. It only deviates where `Board`
has undefined behaviour (garbage resting on row 0 reads `_tiles[-1]`).

What it cannot know: colours of rows that do not exist yet (new buffer rows
after the first, garbage interiors after their first row) come from its own
PRNG, and `inventedColors()` turns true once one is on the board; and garbage
the opponent sends after `import()`.

`tools/claudesim-check` checks it against the real board in lockstep:

```
cmake --build build --target claudesim-check     # not built by default
./build/claudesim-check [seed] [ticks] [window] [swap%]
```

It drives a `Board` with random inputs and random incoming garbage and, every
tick, compares every field (cells and their timers, chain flags, cursor,
score, stack, grace, garbage blocks and queue, spawn positions) with a model
imported one tick earlier and with one imported once per window and fed the
same inputs. It exits 1 on any difference. Planting a one-tick error in the
float timer, or dropping the list wrap-around, makes it fail at once. Run it
after any change to `Board`: the skill chains depend on the model being exact.
`Board::getGarbageSpawnPosition()` and `GarbageBlock::isFalling()` exist so
`import()` can copy that state.

### AI benchmarks (`tools/aibench`)

`bench.py` measures one strategy over a *range of seeds*; `match.py` plays two
strategies against each other with both boards driven by the AI, which is far
more sensitive than "who tops out first".

```
tools/aibench/bench.py  qwen,budget=16 1000 1015 --jobs 8 --frames 6000
tools/aibench/match.py  qwen qwen,followup=0 16 5000 --jobs 8 --frames 6000
```

- One game = one `--debug-stdin --no-render --seed S` process: `goto ai`, the
  countdown cleared with three `wait 1100`/`tick` pairs (see Gotchas), then
  `tick 100` chunks (`--frames`/`--json-every`). `bench.py` drives board 2 and
  leaves board 1 to a keyboard controller that never plays, so what usually
  ends a bench game is *board 1* topping out - `ticks` is not a clean survival
  measure, the `stack` and `gq` columns are. `match.py` seeds are
  `seed-from + game/2` and the seats swap every game, so a pair's record is
  directly comparable between runs.
- Determinism: always pass real seeds (never 0, which means "from the clock").
  Even then the search is only *conditionally* deterministic: `budget` is wall
  clock per plan, so a loaded machine (high `--jobs`) makes every game look
  less deep. Pin `budget` and keep the machine otherwise idle when a number has
  to be reproducible.
- Cost, measured on this machine (`--jobs 1`): a `bench.py` game takes ~10 s of
  single-core CPU - note that it ends after only ~1200 ticks, because board 1
  (idle) tops out - and a `match.py` game (both boards searched, up to the full
  6000 ticks) takes ~2 min, so a 16-game match at 8 jobs is 5-10 minutes.
  In *live* play the AI costs about 1.4x real time (~87 ticks/s instead of the
  62.5 of a 16 ms tick), which is acceptable. Dropping `budget` from 45 to 6
  changes neither the wall time (the ~60-candidate first-ply pass at
  `ticks=420` is the floor, not the follow-up refinements) nor the strength.
  `PANELPOP_AI_DEBUG=1` prints one line per plan on stdout; typical plans see
  ~30 candidates, run 4-6 refinements and take 12-47 ms, one plan every 6-8
  ticks.
- Measured on this machine with `match.py` (8 jobs, 6000 frames, 16 games per
  match unless noted, seats swapped every game):
  - **`qwen` beats `basic` 16-0** (seeds 5000-5007; 11 of the 16 by the
    opponent topping out; mean score 4901 vs 1422) - the search is worth a lot.
  - `qwen` vs `qwen,followup=0`: 9-7; vs `qwen,budget=10`: 9-7 (seeds 2000-2007)
    - the second ply pays, a 4x smaller CPU budget costs little.
  - `qwen` vs `qwen,replan=6,replanActive=24` (planning far less often): 8-8 and
    16-16 in a second, larger run (seeds 3200-3215) - the replan cadence makes
    no measurable difference, so the frequent re-planning is only a CPU cost.
  - `qwen` vs `qwen,ticks=700` (longer horizon): 10-6 - maybe slightly better.
  - `cand` (how many follow-up searches, best candidate first): 6 vs 1 came out
    even (6-10 on seeds 2000-2007, 18-14 over 32 games on seeds 3000-3015),
    while `cand=1` lost to `cand=3` 13-19 (seeds 3100-3115) - do at least a few
    refinements, the exact number is noise.
- `claude` vs `qwen` (4 jobs, 6000 frames, every game decided by a top-out):
  - final defaults: **23-1** on seeds 8000-8011, never used for tuning (the
    loss went to points, 5080-5023); 16-0 on seeds 6000-6007 (used for
    tuning). `claude` vs `basic`: 8-0 (seeds 9000-9003).
  - the steps on the way, same seeds 6000-6007: first version (pace 3/2)
    14-2; counting blocks instead of height as material (a board of garbage
    over a few blocks no longer looks comfortable) plus the `undo` rule 16-0;
    human pace 4/3 14-2 (23-1 on unseen seeds 7000-7011); `scramble` (fight
    on instead of idling when buried) 16-0. Both 14-2 losses were the same
    story: `claude` spent its stack on a 6-chain, `qwen` answered with a 7-chain
    and six rows landed on an empty board.
  - what it looks like (seed 6006, `claude` vs `qwen`, 3500 ticks of json
    snapshots plus `PANELPOP_CLAUDE_DEBUG`): `claude`
    sent a 7-chain and a 6-chain built with 11 skill-chain links, moved its
    cursor ~3 cells/s against `qwen`'s ~8, kept its stack at ~7.7 rows, and
    the model predicted every plan exactly. A `claude` game costs about as much
    CPU as the `qwen` side of it; a 24-game match at 4 jobs takes ~15 min.

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
  A failed `Mix_OpenAudio` (no audio device) is the exception: init then skips
  `loadAudio()`, every `Mix_Chunk`/`Mix_Music` stays null, and the game runs
  silent, since `Mix_PlayChannel()`/`Mix_PlayMusic()` reject null and the
  other mixer calls are no-ops with no device. (Before this was handled,
  `loadAudio()` failed because SDL_mixer won't load files with no device, so
  the xvfb run above exited 1 on boxes with no sound device.) With a device
  open, missing or broken audio files are still fatal.
- **Title screen auto-demo.** 600 idle ticks (`DEMO_TIMEOUT`) start
  `DemoGameState`; a long scripted session on the title screen ends up there.
  The switch goes through `FadeTransition`, which is tick-based
  (`TRANSITION_TICKS = 120`), so it needs 120 more ticks before
  `state` reports `DemoGameState`. `goto` skips fades.
- **Game over needs `Esc`/`Return`** to reset the game (see
  `GameState::tick()`).
- **A swap counts as an active block.** `Board::activeBlocks()` is true while
  any block is swapping, and the top-out grace timer, the automatic stack
  raise and the spawning of queued garbage all wait for no active blocks. So
  a player (or AI) that keeps swapping holds off garbage and stays alive on
  a full board; long chains do the same legitimately. `claude` ends up doing
  this when it is about to top out (see `scramble`) - it looks like
  panicked button mashing, and it is what the rules reward.
- **The AI costs CPU.** `QwenStrat` re-plans as soon as its queued actions run
  out (`replan`/`replanActive`) and its search budget is wall clock, so one
  6000-tick AI game is a few seconds of pure C++ and a benchmark with 8 jobs is
  CPU-bound. Note that a loaded machine makes the anytime search *shallower*:
  results are only comparable when the machine is otherwise idle, or if you pin
  `budget`.
- **`panelpop.ini` gains an `[ai]` section** once the options menu saves a
  strategy; a benchmark or test session that opens that menu dirties it (see
  the ini gotcha above).

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

## Game bugs found while building the AI (fixed on this branch)

Pre-existing bugs, found because the planner simulates the rules and disagrees
with the live board, and because UBSan trips on a scripted session:

- `Board::fillBufferRow()` and `GarbageBlock::fillBufferRow()` read
  `_bufferRow[i - 1]` with `i == 0` — UBSan `index -1 out of bounds for type
  'Tile [6]'` on the base branch (`Game/Board.cpp:84`, on every board
  construction and every stack advance; a real crash under ASan for garbage).
  Guarded with the same `if (i - 1 >= 0)` pattern `Board::fillRandom()` uses.
- `Board::Board()` left the member `_activeBlocks` uninitialised. Nothing
  recomputes it before the first `Board::tick()`, and it gates the stack-raise
  checks, the spawn timers and `hasActiveBlocks()` — a board could start out
  pretending tiles were mid-air. Now initialised to `false`.
- `FadeTransition::tick()` created the next state in `render()` only, so with
  `--no-render` `_nextState` stayed null and the transition never produced a
  state. It is now created in `tick()`.
- The AI stopped playing after the first round of a "VS AI" match (the second
  round, and every one after it). The strategy object lives in the long-lived
  `AIBoardController`, and `QwenStrat::plan()` refuses to search before the
  board's tick counter reaches `_nextPlanTick` - a number the *previous* round
  left in the future of the freshly reset board. Measured with
  `PANELPOP_AI_DEBUG=1` on seed 5000: round 1 ends at `ticks=1233`, the reset
  board then produced no plan at all for its first 1232 ticks (score 0, cursor
  never moved), and the first plan appeared at exactly `ticks=1233`. Added
  `AIStrategy::reset()`, called by `AIBoardController` when the board's tick
  counter goes backwards; rounds 2 and 3 now score like round 1.

With those in, a scripted session (endless with garbage, and a full VS AI game)
is clean: `valgrind --leak-check=full` reports `definitely lost: 0 bytes` (all
that remains is `still reachable` SDL/TTF/ALSA at exit) and a
`-fsanitize=undefined` build reports no runtime errors. Before the fix, UBSan
reported `Game/Board.cpp:84: index -1 out of bounds for type 'Tile [6]'`.

Latent, not fixed:

- `Board::garbageBlockCanFall()` computes `row = gb.getY() - gb.getH()` and
  dereferences `_tiles[row][i]` without checking `row >= 0`, and reads `Block`
  members (`._state`) from tiles that hold a `GarbageBlock`. It never goes
  negative in the sessions run under UBSan/valgrind, but
  `SimBoard::garbageCanFall()` guards the row anyway, so simulated garbage can
  fall slightly more readily than real garbage (`AI/SimBoard.cpp:707`).
