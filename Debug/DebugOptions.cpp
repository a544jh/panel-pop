/*
 * DebugOptions.cpp
 */

#include "DebugOptions.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

DebugOptions::DebugOptions() :
    _headless(false),
    _console(false),
    _render(true),
    _maxTicks(0),
    _seed(0),
    _usage(false) {
}

DebugOptions &DebugOptions::get() {
    static DebugOptions options;
    return options;
}

bool DebugOptions::envFlag(const char *name, bool fallback) {
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }
    return strcmp(value, "0") != 0 && strcmp(value, "false") != 0;
}

int DebugOptions::envInt(const char *name, int fallback) {
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }
    return atoi(value);
}

void DebugOptions::parse(int argc, char *argv[]) {
    _headless = envFlag("PANELPOP_HEADLESS", false);
    _console = envFlag("PANELPOP_DEBUG_STDIN", false);
    _render = envFlag("PANELPOP_RENDER", true);
    _maxTicks = envInt("PANELPOP_MAX_TICKS", 0);
    _seed = envInt("PANELPOP_SEED", 0);

    int kept = 1;
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (!strcmp(arg, "--headless")) {
            _headless = true;
        } else if (!strcmp(arg, "--debug-stdin")) {
            _console = true;
        } else if (!strcmp(arg, "--no-render")) {
            _render = false;
        } else if (!strcmp(arg, "--help") || !strcmp(arg, "-h")) {
            _usage = true;
        } else if (!strncmp(arg, "--frames=", 9)) {
            _maxTicks = atoi(arg + 9);
        } else if (!strcmp(arg, "--frames") && i + 1 < argc) {
            _maxTicks = atoi(argv[++i]);
        } else if (!strncmp(arg, "--seed=", 7)) {
            _seed = atoi(arg + 7);
        } else if (!strcmp(arg, "--seed") && i + 1 < argc) {
            _seed = atoi(argv[++i]);
        } else {
            argv[kept++] = argv[i];
        }
    }
    argv[kept] = nullptr;
    argc = kept;

    // Driving the game from stdin implies headless-friendly defaults.
    if (_console) {
        _headless = true;
    }
}

bool DebugOptions::headless() const {
    return _headless;
}

bool DebugOptions::console() const {
    return _console;
}

bool DebugOptions::render() const {
    return _render;
}

int DebugOptions::maxTicks() const {
    return _maxTicks;
}

int DebugOptions::seed() const {
    return _seed;
}

bool DebugOptions::usageRequested() const {
    return _usage;
}

std::string DebugOptions::usage() {
    return
        "usage: panel-pop [options]\n"
        "\n"
        "debug/headless options:\n"
        "  --headless          run with SDL's dummy video+audio drivers (no display needed)\n"
        "  --debug-stdin       interactive debug console on stdin, one logic tick at a time\n"
        "                      (implies --headless; type 'help' at the prompt)\n"
        "  --frames=<n>        stop after n logic ticks (default: run until quit)\n"
        "  --no-render         skip State::render() while stepping (faster, no render coverage)\n"
        "  --seed=<n>          seed rand() with n instead of the clock, so a\n"
        "                      scripted session deals the same blocks every run\n"
        "  -h, --help          show this help\n"
        "\n"
        "the same options exist as environment variables: PANELPOP_HEADLESS,\n"
        "PANELPOP_DEBUG_STDIN, PANELPOP_RENDER, PANELPOP_MAX_TICKS.\n"
        "See AGENTS.md for the debug console command reference.\n";
}
