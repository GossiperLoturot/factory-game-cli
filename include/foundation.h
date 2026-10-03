#ifndef _FOUNDATION_H
#define _FOUNDATION_H

#include <string_view>
#include <vector>

enum class Item {
  WATER,
  HYDROGEN,
  OXYGEN,

  SILICON,
  SILICON_WAFER,
  CIRCUIT_WAFER,
  CIRCUIT,
  SOLDERING_IRON,
  CIRCUIT_BOARD,
  CHIP,
};

std::string_view item_to_string(Item item);

class EvaluateContext {
 public:
  int stage;
  int design_time;
  std::vector<Item> items;
  std::vector<int> counts;
};

#endif  // _FOUNDATION_H
