#!/usr/bin/env python3
"""Deterministic head-to-head AI matches (see AGENTS.md "AI benchmarks").

  python3 tools/aibench/match.py <specA> <specB> [games] [seed-from] [--jobs J]

Each game is an "VS AI" state whose *both* boards are computer controlled
("strategy A 1" replaces the human keyboard controller with an AI). The round
ends as soon as one board tops out (VsGame::handleEnd()); the other board wins
the round. Games alternate which side gets which strategy, so the result is not
skewed by board-side effects (the board geometry is symmetric, but the cursor
start and the garbage-send order are not).

Seed i uses seed = seed-from + i/2, so both halves of a pair see the same
opening boards, and a strategy pair's record is directly comparable across runs.

In the per-game listing b1/b2 are the strategies on boards 1 and 2 (they swap
every game); "winner=A/B" refers to the first/second strategy argument.
"""
import json
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

BIN = "./build/panel-pop"
FRAMES = 6000
CHUNK = 100


def play(game, seed, specA, specB, frames=FRAMES):
    a, b = (specA, specB) if game % 2 == 0 else (specB, specA)
    cmds = ["goto ai", "strategy %s 1" % a, "strategy %s 2" % b]
    for _ in range(3):
        cmds += ["wait 1100", "tick"]
    for _ in range(frames // CHUNK):
        cmds += ["tick %d" % CHUNK, "json"]
    cmds.append("quit")
    p = subprocess.run([BIN, "--debug-stdin", "--no-render", "--seed", str(seed)],
                       input="\n".join(cmds), capture_output=True, text=True)
    snaps = [json.loads(l) for l in p.stdout.splitlines() if l.startswith("{")]
    if not snaps:
        raise SystemExit("no json output\n" + p.stdout[-400:] + p.stderr[-400:])
    last = snaps[-1]
    b1 = next(x for x in last["boards"] if x["player"] == 1)
    b2 = next(x for x in last["boards"] if x["player"] == 2)
    d1 = b1["boardState"] == "GAME_OVER"
    d2 = b2["boardState"] == "GAME_OVER"
    if d1 and not d2:
        w, how = b, "top"                     # board 1 topped out
    elif d2 and not d1:
        w, how = a, "top"
    else:                                     # frame cap: decide on score
        w, how = (a, "score") if b1["score"] > b2["score"] else \
                 ((b, "score") if b2["score"] > b1["score"] else (None, "draw"))
    return dict(game=game, seed=seed, a=a, b=b, winner=w, how=how,
                ticks=last.get("ticks", b1["ticksRun"]),
                s1=b1["score"], s2=b2["score"])


def main():
    specA, specB = sys.argv[1], sys.argv[2]
    games = int(sys.argv[3]) if len(sys.argv) > 3 else 16
    seed0 = int(sys.argv[4]) if len(sys.argv) > 4 else 1000
    jobs, frames = 1, FRAMES
    for flag, key in (("--jobs", "jobs"), ("--frames", "frames")):
        if flag in sys.argv:
            v = int(sys.argv[sys.argv.index(flag) + 1])
            if key == "jobs":
                jobs = v
            else:
                frames = v
    tasks = [(g, seed0 + g // 2) for g in range(games)]
    with ThreadPoolExecutor(max_workers=jobs) as ex:
        res = list(ex.map(lambda t: play(t[0], t[1], specA, specB, frames), tasks))
    wa = sum(1 for r in res if r["winner"] == specA)
    wb = sum(1 for r in res if r["winner"] == specB)
    draws = len(res) - wa - wb
    tops = sum(1 for r in res if r["how"] == "top")
    print("%-30s %2d-%-2d (%5.1f%%) vs %-30s  draws=%d  decided-by-top-out=%d  frames=%d  seeds=%d..%d"
          % (specA, wa, wb, 100.0 * wa / max(1, wa + wb), specB, draws, tops, frames,
             seed0, seed0 + (games - 1) // 2))
    for r in res:
        print("   g%-2d seed=%d b1=%-26s b2=%-26s winner=%-6s %-5s ticks=%d scores=%d/%d"
              % (r["game"], r["seed"], r["a"], r["b"],
                 {None: "draw", specA: "A", specB: "B"}[r["winner"]], r["how"],
                 r["ticks"], r["s1"], r["s2"]))


main()
