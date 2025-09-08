#include "machine.h"

#include <algorithm>

namespace factory_game {

// ANCHOR

Anchor::Anchor() : m_parent(nullptr), m_pipes(), m_piped_anchors() {}

Anchor::Anchor(Machine* parent)
    : m_parent(parent), m_pipes(), m_piped_anchors() {}

Anchor::~Anchor() = default;

// SPATIAL IDX

MachineSpatialIdx::MachineSpatialIdx(std::unordered_map<glm::ivec2, std::shared_ptr<Machine>>& spatial_idx, std::shared_ptr<Machine>& cursor) : m_spatial_idx(spatial_idx), m_cursor(cursor) {}

MachineSpatialIdx::~MachineSpatialIdx() = default;

void MachineSpatialIdx::Write(glm::ivec2 point) {
  m_spatial_idx.insert_or_assign(point, m_cursor);
}

// BASE MACHINE

Machine::Machine(glm::ivec2 point) : m_point(point) {}

Machine::~Machine() = default;

// INPUT MACHINE

InputM::InputM(glm::ivec2 point, Item item)
    : Machine(point), item(item), m_output(this) {}

InputM::~InputM() = default;

bool InputM::is_breakable() { return false; }

void InputM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Input]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void InputM::build_spatial_idx(MachineSpatialIdx writer) {}

void InputM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void InputM::upgrade_anchors() { m_output.m_parent = this; }

std::vector<Anchor*> InputM::get_anchors() { return {&m_output}; }

void InputM::insert_item(Item given_item) {  }

void InputM::evaluate(EvaluateContext* stats, std::default_random_engine rng) {
  auto anchors = m_output.m_piped_anchors;
  if (!anchors.empty()) {
    std::uniform_int_distribution<> dist(0,
                                         static_cast<int>(anchors.size() - 1));
    Anchor* target = anchors[dist(rng)];
    target->m_parent->insert_item(item);
  }
}

// OUTPUT MACHINE

OutputM::OutputM(glm::ivec2 point, Item item)
    : Machine(point), item(item), m_input(this), m_stored_count(0) {}

OutputM::~OutputM() = default;


bool OutputM::is_breakable() { return false; }


void OutputM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y + 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Output]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
}

void OutputM::build_spatial_idx(MachineSpatialIdx writer) {  }

void OutputM::upgrade_anchors() { m_input.m_parent = this; }

std::vector<Anchor*> OutputM::get_anchors() { return {&m_input}; }

void OutputM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input;
}

void OutputM::insert_item(Item given_item) {
  if (given_item == item) {
    m_stored_count++;
  }
}

void OutputM::evaluate(EvaluateContext* stats, std::default_random_engine rng) {
  rng;

  auto it = find(stats->items.begin(), stats->items.end(), item);
  if (it != stats->items.end()) {
    size_t index = distance(stats->items.begin(), it);
    stats->counts[index] += m_stored_count;
  } else {
    stats->items.push_back(item);
    stats->counts.push_back(m_stored_count);
  }
}

// ELECTROLYZER MACHINE

ElectrolyzerM::ElectrolyzerM(glm::ivec2 point)
    : Machine(point),
      m_input(this),
      m_output0(this),
      m_output1(this),
      m_stored_count(0) {}

ElectrolyzerM::~ElectrolyzerM() = default;


bool ElectrolyzerM::is_breakable() { return true; }


void ElectrolyzerM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Electrolyzer]]");
  draw_manager->draw_label(m_point.x + 7, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O1");
  draw_manager->draw_label(m_point.x + 10, m_point.y + 1, "O2");
}

void ElectrolyzerM::build_spatial_idx(MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

void ElectrolyzerM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output0.m_parent = this;
  m_output1.m_parent = this;
}

std::vector<Anchor*> ElectrolyzerM::get_anchors() {
  return {&m_input, &m_output0, &m_output1};
}

void ElectrolyzerM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 7] = &m_input;
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output0;
  buffer[(m_point.y + 1) * 120 + m_point.x + 10] = &m_output1;
}

void ElectrolyzerM::insert_item(Item given_item) {
  if (given_item == ITEM_WATER) {
    m_stored_count++;
  }
}

void ElectrolyzerM::evaluate(EvaluateContext* stats,
                             std::default_random_engine rng) {
  stats;

  if (m_stored_count > 0) {
    auto anchors0 = m_output0.m_piped_anchors;
    if (!anchors0.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors0.size() - 1));
      Anchor* target = anchors0[dist(rng)];
      target->m_parent->insert_item(ITEM_HYDROGEN);
    }

    auto anchors1 = m_output1.m_piped_anchors;
    if (!anchors1.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors1.size() - 1));
      Anchor* target = anchors1[dist(rng)];
      target->m_parent->insert_item(ITEM_OXYGEN);
    }

    m_stored_count--;
  }
}

// CUTTER MACHINE

CutterM::CutterM(glm::ivec2 point)
    : Machine(point), m_input(this), m_output(this), m_stored_count() {}

CutterM::~CutterM() = default;

bool CutterM::is_breakable() { return true; }


void CutterM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Cutter]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void CutterM::build_spatial_idx(MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

void CutterM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output.m_parent = this;
}

std::vector<Anchor*> CutterM::get_anchors() { return {&m_input, &m_output}; }

void CutterM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input;
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void CutterM::insert_item(Item given_item) {
  if (given_item == ITEM_SILICON) {
    m_stored_count[0]++;
  }
  if (given_item == ITEM_CIRCUIT_WAFER) {
    m_stored_count[1]++;
  }
}

void CutterM::evaluate(EvaluateContext* stats, std::default_random_engine rng) {
  stats;

  if (m_stored_count[0] > 0) {
    auto anchors0 = m_output.m_piped_anchors;
    if (!anchors0.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors0.size() - 1));
      Anchor* target = anchors0[dist(rng)];
      target->m_parent->insert_item(ITEM_SILICON_WAFER);
    }
    m_stored_count[0]--;
  }

  if (m_stored_count[1] > 0) {
    auto anchors0 = m_output.m_piped_anchors;
    if (!anchors0.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors0.size() - 1));
      Anchor* target = anchors0[dist(rng)];
      target->m_parent->insert_item(ITEM_CIRCUIT);
    }
    m_stored_count[1]--;
  }
}

// LAZER MACHINE

LaserM::LaserM(glm::ivec2 point)
    : Machine(point), m_input(this), m_output(this), m_stored_count(0) {}

LaserM::~LaserM() = default;



bool LaserM::is_breakable() { return true; }

void LaserM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Laser]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void LaserM::build_spatial_idx(MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

void LaserM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output.m_parent = this;
}

std::vector<Anchor*> LaserM::get_anchors() { return {&m_input, &m_output}; }

void LaserM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input;
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void LaserM::insert_item(Item given_item) {
  if (given_item == ITEM_SILICON_WAFER) {
    m_stored_count++;
  }
}

void LaserM::evaluate(EvaluateContext* stats, std::default_random_engine rng) {
  stats;

  if (m_stored_count > 0) {
    auto anchors0 = m_output.m_piped_anchors;
    if (!anchors0.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors0.size() - 1));
      Anchor* target = anchors0[dist(rng)];
      target->m_parent->insert_item(ITEM_CIRCUIT_WAFER);
    }

    m_stored_count--;
  }
}

// ASSEMBLER MACHINE

AssemblerM::AssemblerM(glm::ivec2 point)
    : Machine(point),
      m_input0(this),
      m_input1(this),
      m_input2(this),
      m_output(this),
      m_stored_count() {}

AssemblerM::~AssemblerM() = default;


bool AssemblerM::is_breakable() { return true; }


void AssemblerM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Assembler]]");
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, "I1");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I2");
  draw_manager->draw_label(m_point.x + 8, m_point.y - 1, "I3");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void AssemblerM::build_spatial_idx(MachineSpatialIdx writer) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      writer.Write(glm::ivec2(x, y));
    }
  }
}

void AssemblerM::upgrade_anchors() {
  m_input0.m_parent = this;
  m_input1.m_parent = this;
  m_input2.m_parent = this;
  m_output.m_parent = this;
}

std::vector<Anchor*> AssemblerM::get_anchors() {
  return {&m_input0, &m_input1, &m_input2, &m_output};
}

void AssemblerM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 2] = &m_input0;
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input1;
  buffer[(m_point.y - 1) * 120 + m_point.x + 8] = &m_input2;
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void AssemblerM::insert_item(Item given_item) {
  if (given_item == ITEM_CIRCUIT) {
    m_stored_count[0]++;
  }
  if (given_item == ITEM_SOLDERING_IRON) {
    m_stored_count[1]++;
  }
  if (given_item == ITEM_CIRCUIT_BOARD) {
    m_stored_count[2]++;
  }
}

void AssemblerM::evaluate(EvaluateContext* stats,
                          std::default_random_engine rng) {
  stats;

  if (m_stored_count[0] > 0 && m_stored_count[1] > 0 && m_stored_count[2] > 0) {
    auto anchors0 = m_output.m_piped_anchors;
    if (!anchors0.empty()) {
      std::uniform_int_distribution<> dist(
          0, static_cast<int>(anchors0.size() - 1));
      Anchor* target = anchors0[dist(rng)];
      target->m_parent->insert_item(ITEM_CHIP);
    }
    m_stored_count[0]--;
    m_stored_count[1]--;
    m_stored_count[2]--;
  }
}

// MACHINE MANAGER

MachineManager::MachineManager()
    : m_machines(), m_spatial_idx(), m_anchor_idx(120 * 30) {}

MachineManager::~MachineManager() = default;

void MachineManager::add_machine(std::shared_ptr<Machine> machine) {
  m_machines.insert(machine);
  build_spatial_idx();
}

void MachineManager::build_spatial_idx() {
  m_spatial_idx.clear();

  for (auto machine : m_machines) {
    auto writer = MachineSpatialIdx(m_spatial_idx, machine);
    machine->build_spatial_idx(writer);
  }
}

void MachineManager::remove_machine(std::shared_ptr<Machine> machine) {
  m_machines.erase(machine);
  build_spatial_idx();
}

std::shared_ptr<Machine> MachineManager::find_machine(glm::ivec2 point) {
  auto it = m_spatial_idx.find(point);
  if (it == m_spatial_idx.end()) return nullptr;
  return it->second;
}

void MachineManager::draw(DrawManagerBase* draw_manager) {
  for (auto machine : m_machines) {
    machine->draw(draw_manager);
  }
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
Anchor* MachineManager::get_anchor(glm::ivec2 point) {
  for (size_t i = 0; i < 120 * 30; ++i) {
    m_anchor_idx[i] = nullptr;
  }

  for (auto machine : m_machines) {
    machine->create_anchor(m_anchor_idx.data());
  }

  return m_anchor_idx[point.y * 120 + point.x];
}

void MachineManager::evaluate(EvaluateContext* stats,
                              std::default_random_engine rng) {
  for (auto machine : m_machines) {
    machine->evaluate(stats, rng);
  }
}

}  // namespace factory_game