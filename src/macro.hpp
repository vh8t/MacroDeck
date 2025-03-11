#pragma once

#include "action.hpp"

#include <vector>

struct Macro {
  std::vector<Action> macro;

  void run() const {
    for (const auto &action : macro) {
      action.execute();
    }
  }
};
