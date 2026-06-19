/*
 * Paths.cpp
 *
 *  See Paths.h for an overview of the lookup rules.
 */

#include "Paths.h"

#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define PP_MKDIR(p) _mkdir((p).c_str())
#else
#define PP_MKDIR(p) mkdir((p).c_str(), 0755)
#endif

// Fallback for builds that don't define PANELPOP_DATADIR (e.g. plain `make`
// from the source tree). "." keeps the historical relative-path behaviour.
#ifndef PANELPOP_DATADIR
#define PANELPOP_DATADIR "."
#endif

namespace {

bool isDir(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR);
}

bool isFile(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFREG);
}

const std::string &assetBase() {
    // Computed once: prefer ./assets from the working directory (running from
    // the source tree), otherwise the installed data dir.
    static const std::string base = isDir("assets") ? std::string(".")
                                                     : std::string(PANELPOP_DATADIR);
    return base;
}

std::string env(const char *name) {
    const char *value = std::getenv(name);
    return (value != nullptr) ? std::string(value) : std::string();
}

std::string computeConfigFile() {
    // A panelpop.ini next to the binary's working dir wins (portable use).
    if (isFile("panelpop.ini")) {
        return "panelpop.ini";
    }

    std::string configHome = env("XDG_CONFIG_HOME");
    if (configHome.empty()) {
        std::string home = env("HOME");
        if (!home.empty()) {
            configHome = home + "/.config";
        }
    }

    if (configHome.empty()) {
        // No usable home directory; fall back to the working directory.
        return "panelpop.ini";
    }

    PP_MKDIR(configHome);
    std::string dir = configHome + "/panel-pop";
    PP_MKDIR(dir);
    return dir + "/panelpop.ini";
}

} // namespace

namespace Paths {

std::string asset(const std::string &relative) {
    const std::string &base = assetBase();
    if (base == ".") {
        return relative;
    }
    return base + "/" + relative;
}

std::string configFile() {
    static const std::string path = computeConfigFile();
    return path;
}

} // namespace Paths
