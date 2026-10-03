#include "foundation.h"

std::string_view item_to_string(Item item) {
  switch (item) {
    case Item::WATER:
      return "Water";
    case Item::HYDROGEN:
      return "Hydrogen";
    case Item::OXYGEN:
      return "Oxygen";

    case Item::SILICON:
      return "Silicon";
    case Item::SILICON_WAFER:
      return "Silicon Wafer";
    case Item::CIRCUIT_WAFER:
      return "Circuit Wafer";
    case Item::CIRCUIT:
      return "Circuit";
    case Item::SOLDERING_IRON:
      return "Soldering Iron";
    case Item::CIRCUIT_BOARD:
      return "Circuit Board";
    case Item::CHIP:
      return "Chip";

    default:
      return "Unknown";
  }
}
