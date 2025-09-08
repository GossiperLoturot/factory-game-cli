#pragma once

#include <string>
#include <vector>

namespace factory_game {

enum Item {
  ITEM_WATER,
  ITEM_HYDROGEN,
  ITEM_OXYGEN,

  ITEM_SILICON,
  ITEM_SILICON_WAFER,
  ITEM_CIRCUIT_WAFER,
  ITEM_CIRCUIT,
  ITEM_SOLDERING_IRON,
  ITEM_CIRCUIT_BOARD,
  ITEM_CHIP,
};

std::string item_to_string(Item item);

struct EvaluateContext {
  int stage;
  int design_time;
  std::vector<Item> items;
  std::vector<int> counts;
};


}  // namespace factory_game