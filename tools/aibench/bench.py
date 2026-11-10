#!/usr/bin/env python3
"""Deterministic single-board AI benchmark (see AGENTS.md "AI benchmarks").

  python3 tools/aibench/bench.py <strategy> [seed-from] [seed-to]
                                 [--frames N] [--json-every N] [--jobs J]

Starts an "VS AI" game (board 2 is the AI under test, board 1 is an idle
keyboard player), steps it deterministically and reads `json` snapshots.
Everything is seed- and tick-deterministic; `wait 1100` three times clears the
wall-clock countdown (see the countdown gotcha in AGENTS.md).

A strategy spec may carry tuning knobs: "qwen,budget=45,height=1.5". The knobs
are listed in QwenStrat::setOptions() (ticks, followup, cand, budget, replan,
replanActive, followupWeight, travel, score, garbage, chainLevel, pendingScore,
height, heightSquare, holes, garbageRows, panicBlocks, topThreat, matchOptions,
optionBlocks, adjacency, columnSpread, raisePenalty, death).

Reported per game (board 2):
  ticks   board ticksRun when the run ended (survival; capped by --frames)
  score   board score
  panic%  fraction of samples with board panic=true (the AI sees an
          unwinnable stack; survival configs often end the game early)
  gq      mean/max garbageQueued (pending garbage rows about to land; the AI
          removes these by matching, lower is better)
  stack   mean final / max highest occupied visible row (11 = one row from
          dying), i.e. how much pressure the AI was under

Note: "ticks" is capped by board 1 topping out, and a stronger AI sends more
garbage there, so ticks is *not* a clean survival measure - use match.py for
wins/losses and "stack"/"gq" for pressure.
"""
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

BIN = "./build/panel-pop"


def parse_args(argv):
    opts = {"frames": 4000, "every": 100, "jobs": 1}
    i = 0
    while i < len(argv):
        if argv[i] == "--frames":
            opts["frames"] = int(argv[i + 1]); i += 2
        elif argv[i] == "--json-every":
            opts["every"] = int(argv[i + 1]); i += 2
        elif argv[i] == "--jobs":
            opts["jobs"] = int(argv[i + 1]); i += 2
        else:
            raise SystemExit("unknown option %r" % argv[i])
    return opts


def top_row(board):
    """Highest visible row holding anything; grid rows are printed top first."""
    for i, row in enumerate(board["grid"]):
        if row.strip("."):
            return len(board["grid"]) - 1 - i
    return -1


def run(seed, spec, opts):
    # The whole spec (key plus "key=value" knobs) is one console argument.
    cmds = ["goto ai", "strategy %s 2" % spec]
    for _ in range(3):                      # three 1000 ms countdown steps
        cmds += ["wait 1100", "tick"]
    for _ in range(opts["frames"] // opts["every"]):
        cmds += ["tick %d" % opts["every"], "json"]
    cmds.append("quit")
    p = subprocess.run([BIN, "--debug-stdin", "--no-render", "--seed", str(seed)],
                       input="\n".join(cmds), capture_output=True, text=True)
    snaps = [json.loads(l) for l in p.stdout.splitlines() if l.startswith("{")]
    if not snaps:
        raise SystemExit("no json output (build/run problem?)\n" + p.stdout[-500:] + p.stderr[-500:])
    rows = [b for s in snaps for b in s["boards"] if b["player"] == 2]
    ticks = rows[-1]["ticksRun"]
    score = rows[-1]["score"]
    panic = sum(1 for b in rows if b["panic"]) / len(rows) * 100.0
    gq = [b["garbageQueued"] for b in rows]
    dead = rows[-1]["boardState"] == "GAME_OVER"
    tops = [top_row(b) for b in rows]
    return dict(seed=seed, ticks=ticks, score=score, panic=panic,
                gq=sum(gq) / len(gq), gqmax=max(gq), dead=dead, top=tops[-1],
                topmax=max(tops), state=rows[-1]["boardState"])


def main():
    spec = sys.argv[1]
    a = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    b = int(sys.argv[3]) if len(sys.argv) > 3 else 1015
    opts = parse_args(sys.argv[4:])
    with ThreadPoolExecutor(max_workers=opts["jobs"]) as ex:
        res = list(ex.map(lambda s: run(s, spec, opts), range(a, b + 1)))
    n = len(res)
    print("strategy=%s seeds=%d..%d frames=%d jobs=%d" % (spec, a, b, opts["frames"], opts["jobs"]))
    print("  ticks %6.0f   score %6.1f   panic%% %5.1f   gq %4.2f/%2d   stack %4.2f/%2d   topped out %d/%d"
          % (sum(r["ticks"] for r in res) / n, sum(r["score"] for r in res) / n,
             sum(r["panic"] for r in res) / n, sum(r["gq"] for r in res) / n,
             max(r["gqmax"] for r in res), sum(r["top"] for r in res) / n,
             max(r["topmax"] for r in res), sum(1 for r in res if r["dead"]), n))
    print("  ticks:", " ".join(str(r["ticks"]) for r in res))
    print("  score:", " ".join(str(r["score"]) for r in res))


main()
