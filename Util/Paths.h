/*
 * Paths.h
 *
 * Helpers for locating bundled assets and the user's config file.
 *
 * Assets are looked up relative to the current working directory first (so a
 * build can be run straight from the source tree), and fall back to the
 * compile-time install data dir (PANELPOP_DATADIR, e.g. /usr/share/panel-pop).
 *
 * The config file lives in the XDG config dir (~/.config/panel-pop), unless a
 * panelpop.ini already exists in the working directory, which then takes
 * precedence (portable / development use).
 */

#ifndef UTIL_PATHS_H_
#define UTIL_PATHS_H_

#include <string>

namespace Paths {

// Resolve a bundled asset path, e.g. asset("assets/sprites.png").
std::string asset(const std::string &relative);

// Absolute path to the user's config file. Ensures the parent directory exists.
std::string configFile();

} // namespace Paths

#endif /* UTIL_PATHS_H_ */
