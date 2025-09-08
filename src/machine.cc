#include "machine.h"

#include <algorithm>

namespace factory_game {

// SPATIAL IDX

MachineSpatialIdx::MachineSpatialIdx(
    std::unordered_map<glm::ivec2, std::shared_ptr<Machine>>& spatial_idx,
    std::shared_ptr<Machine>& cursor)
    : m_spatial_idx(spatial_idx), m_cursor(cursor) {}

MachineSpatialIdx::~MachineSpatialIdx() = default;

void MachineSpatialIdx::Write(const glm::ivec2 point) const {
  m_spatial_idx.insert_or_assign(point, m_cursor);
}

// BASE MACHINE

Machine::Machine(const glm::ivec2 point) : m_point(point) {}

Machine::~Machine() = default;

// INPUT DUCT

InputDuct::InputDuct(const glm::ivec2 point, const Item item)
    : Machine(point), item(item) {}

InputDuct::~InputDuct() = default;

bool InputDuct::is_breakable() { return false; }

void InputDuct::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Input]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void InputDuct::build_spatial_idx(MachineSpatialIdx writer) {}

// OUTPUT DUCT

OutputDuct::OutputDuct(const glm::ivec2 point, const Item item)
    : Machine(point), item(item) {}

OutputDuct::~OutputDuct() = default;

bool OutputDuct::is_breakable() { return false; }

void OutputDuct::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y + 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Output]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
}

void OutputDuct::build_spatial_idx(MachineSpatialIdx writer) {}

// ELECTROLYZER MACHINE

Electrolyzer::Electrolyzer(const glm::ivec2 point) : Machine(point) {}

Electrolyzer::~Electrolyzer() = default;

bool Electrolyzer::is_breakable() { return true; }

void Electrolyzer::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Electrolyzer]]");
  draw_manager->draw_label(m_point.x + 7, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O1");
  draw_manager->draw_label(m_point.x + 10, m_point.y + 1, "O2");
}

void Electrolyzer::build_spatial_idx(const MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

// CUTTER MACHINE

Cutter::Cutter(const glm::ivec2 point) : Machine(point) {}

Cutter::~Cutter() = default;

bool Cutter::is_breakable() { return true; }

void Cutter::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Cutter]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void Cutter::build_spatial_idx(const MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

// LAZER MACHINE

Laser::Laser(const glm::ivec2 point) : Machine(point) {}

Laser::~Laser() = default;

bool Laser::is_breakable() { return true; }

void Laser::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Laser]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void Laser::build_spatial_idx(const MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

// ASSEMBLER MACHINE

Assembler::Assembler(const glm::ivec2 point) : Machine(point) {}

Assembler::~Assembler() = default;

bool Assembler::is_breakable() { return true; }

void Assembler::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Assembler]]");
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, "I1");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I2");
  draw_manager->draw_label(m_point.x + 8, m_point.y - 1, "I3");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void Assembler::build_spatial_idx(const MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

// MACHINE MANAGER

MachineManager::MachineManager() {}

MachineManager::~MachineManager() = default;

void MachineManager::add_machine(const std::shared_ptr<Machine>& machine) {
  m_machines.insert(machine);
  build_spatial_idx();
}

void MachineManager::build_spatial_idx() {
  m_spatial_idx.clear();

  for (auto machine : m_machines) {
    const auto writer = MachineSpatialIdx(m_spatial_idx, machine);
    machine->build_spatial_idx(writer);
  }
}

void MachineManager::remove_machine(const std::shared_ptr<Machine>& machine) {
  m_machines.erase(machine);
  build_spatial_idx();
}

std::shared_ptr<Machine> MachineManager::find_machine(const glm::ivec2 point) {
  const auto it = m_spatial_idx.find(point);
  if (it == m_spatial_idx.end()) return nullptr;
  return it->second;
}

void MachineManager::draw(DrawManagerBase* draw_manager) const {
  for (const auto machine : m_machines) {
    machine->draw(draw_manager);
  }
}

}  // namespace factory_game