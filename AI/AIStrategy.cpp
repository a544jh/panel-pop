/*
 * AIStrategy.cpp
 */

#include "AIStrategy.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "BasicStrategy.h"
#include "ProStrategy.h"
#include "QwenStrat.h"

namespace {

struct Descriptor {
  const char *key;
  const char *name;
};

// Menu order; the default strategy comes first (see defaultIndex()).
const Descriptor DESCRIPTORS[] = {
    {"qwen", "QwenStrat"},
    {"pro", "Pro"},
    {"basic", "Basic"},
};

} // namespace

AIStrategy::~AIStrategy() {
}

void AIStrategy::setOptions(const std::string &) {
}

void AIStrategy::reset() {
}

const std::vector<std::string> &AIStrategies::keys() {
  static std::vector<std::string> keys;
  if (keys.empty()) {
    for (const Descriptor &d : DESCRIPTORS) {
      keys.push_back(d.key);
    }
  }
  return keys;
}

const std::vector<std::string> &AIStrategies::names() {
  static std::vector<std::string> names;
  if (names.empty()) {
    for (const Descriptor &d : DESCRIPTORS) {
      names.push_back(d.name);
    }
  }
  return names;
}

size_t AIStrategies::defaultIndex() {
  return 0; // "qwen"
}

std::string AIStrategies::defaultKey() {
  return keys().at(defaultIndex());
}

std::unique_ptr<AIStrategy> AIStrategies::create(const std::string &spec) {
  std::string key = spec;
  std::string options;
  const size_t comma = spec.find(',');
  if (comma != std::string::npos) {
    key = spec.substr(0, comma);
    options = spec.substr(comma + 1);
  }

  std::unique_ptr<AIStrategy> strategy;
  if (key == "basic") {
    strategy.reset(new BasicStrategy());
  } else if (key == "pro") {
    strategy.reset(new ProStrategy());
  } else {
    strategy.reset(new QwenStrat());
  }
  strategy->setOptions(options);
  return strategy;
}
