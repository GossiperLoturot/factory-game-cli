#pragma once

#include <string>

namespace factory_game {

struct Point {
 public:
  int x;
  int y;

  Point(int x, int y) : x(x), y(y) {}
};

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

}  // namespace factory_game