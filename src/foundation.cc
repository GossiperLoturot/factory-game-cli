#include "foundation.h"

std::string_view machine_to_string(MachineKind kind) {
  switch (kind) {
    case MachineKind::ELECTROLYZER:
      return "Electrolyzer";
    case MachineKind::CUTTER:
      return "Cutter";
    case MachineKind::LAZER:
      return "Lazer";
    case MachineKind::ASSEMBLER:
      return "Assembler";

    default:
      return "Unknown";
  }
}

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

EvaluateContext::EvaluateContext() : m_stage{}, m_design_time{}, m_items{}, m_rng{std::random_device{}()} {
}
