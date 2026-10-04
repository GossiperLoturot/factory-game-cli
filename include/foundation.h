#ifndef FOUNDATION_H
#define FOUNDATION_H

#include <random>
#include <string_view>
#include <unordered_map>

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
  int m_stage;
  int m_design_time;
  std::unordered_map<Item, int> m_items;
  std::mt19937 m_rng;

  EvaluateContext();
};

#endif  // FOUNDATION_H
