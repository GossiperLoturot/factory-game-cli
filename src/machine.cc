#include "machine.h"

#include <algorithm>

namespace factory_game {

Anchor::Anchor() : m_parent(nullptr), m_pipes(), m_piped_anchors() {}

Anchor::Anchor(Machine* parent)
    : m_parent(parent), m_pipes(), m_piped_anchors() {}

Anchor::~Anchor() = default;

// BASE MACHINE

Machine::Machine(Point point) : m_point(point) {}

Machine::~Machine() = default;

// INPUT MACHINE

InputM::InputM(Point point, Item item)
    : Machine(point), item(item), m_output(this) {}

InputM::~InputM() = default;

void InputM::upgrade_anchors() { m_output.m_parent = this; }

bool InputM::is_breakable() { return false; }

std::string InputM::get_name() { return "Input"; }

std::vector<Anchor*> InputM::get_anchors() { return {&m_output}; }

void InputM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Input]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void InputM::create_idx(size_t* buffer, size_t self_idx) { buffer, self_idx; }

void InputM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void InputM::insert_item(Item given_item) { given_item; }

void InputM::evaluate(ProductiveStats* stats, std::default_random_engine rng) {
  stats;

  auto anchors = m_output.m_piped_anchors;
  if (!anchors.empty()) {
    std::uniform_int_distribution<> dist(0,
                                         static_cast<int>(anchors.size() - 1));
    Anchor* target = anchors[dist(rng)];
    target->m_parent->insert_item(item);
  }
}

// OUTPUT MACHINE

OutputM::OutputM(Point point, Item item)
    : Machine(point), item(item), m_input(this), m_stored_count(0) {}

OutputM::~OutputM() = default;

void OutputM::upgrade_anchors() { m_input.m_parent = this; }

bool OutputM::is_breakable() { return false; }

std::string OutputM::get_name() { return "Output"; }

std::vector<Anchor*> OutputM::get_anchors() { return {&m_input}; }

void OutputM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x + 2, m_point.y + 1, item_to_string(item));
  draw_manager->draw_label(m_point.x, m_point.y, "[[Output]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
}

void OutputM::create_idx(size_t* buffer, size_t self_idx) { buffer, self_idx; }

void OutputM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input;
}

void OutputM::insert_item(Item given_item) {
  if (given_item == item) {
    m_stored_count++;
  }
}

void OutputM::evaluate(ProductiveStats* stats, std::default_random_engine rng) {
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

ElectrolyzerM::ElectrolyzerM(Point point)
    : Machine(point),
      m_input(this),
      m_output0(this),
      m_output1(this),
      m_stored_count(0) {}

ElectrolyzerM::~ElectrolyzerM() = default;

void ElectrolyzerM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output0.m_parent = this;
  m_output1.m_parent = this;
}

bool ElectrolyzerM::is_breakable() { return true; }

std::string ElectrolyzerM::get_name() { return "Electrolyzer"; }

std::vector<Anchor*> ElectrolyzerM::get_anchors() {
  return {&m_input, &m_output0, &m_output1};
}

void ElectrolyzerM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Electrolyzer]]");
  draw_manager->draw_label(m_point.x + 7, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O1");
  draw_manager->draw_label(m_point.x + 10, m_point.y + 1, "O2");
}

void ElectrolyzerM::create_idx(size_t* buffer, size_t self_idx) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      buffer[y * 120 + x] = self_idx;
    }
  }
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

void ElectrolyzerM::evaluate(ProductiveStats* stats,
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

CutterM::CutterM(Point point)
    : Machine(point), m_input(this), m_output(this), m_stored_count() {}

CutterM::~CutterM() = default;

void CutterM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output.m_parent = this;
}

bool CutterM::is_breakable() { return true; }

std::string CutterM::get_name() { return "Cutter"; }

std::vector<Anchor*> CutterM::get_anchors() { return {&m_input, &m_output}; }

void CutterM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Cutter]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void CutterM::create_idx(size_t* buffer, size_t self_idx) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      buffer[y * 120 + x] = self_idx;
    }
  }
}

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

void CutterM::evaluate(ProductiveStats* stats, std::default_random_engine rng) {
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

LaserM::LaserM(Point point)
    : Machine(point), m_input(this), m_output(this), m_stored_count(0) {}

LaserM::~LaserM() = default;

void LaserM::upgrade_anchors() {
  m_input.m_parent = this;
  m_output.m_parent = this;
}

bool LaserM::is_breakable() { return true; }

std::string LaserM::get_name() { return "Laser"; }

std::vector<Anchor*> LaserM::get_anchors() { return {&m_input, &m_output}; }

void LaserM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Laser]]");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void LaserM::create_idx(size_t* buffer, size_t self_idx) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      buffer[y * 120 + x] = self_idx;
    }
  }
}

void LaserM::create_anchor(Anchor** buffer) {
  buffer[(m_point.y - 1) * 120 + m_point.x + 5] = &m_input;
  buffer[(m_point.y + 1) * 120 + m_point.x + 5] = &m_output;
}

void LaserM::insert_item(Item given_item) {
  if (given_item == ITEM_SILICON_WAFER) {
    m_stored_count++;
  }
}

void LaserM::evaluate(ProductiveStats* stats, std::default_random_engine rng) {
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

AssemblerM::AssemblerM(Point point)
    : Machine(point),
      m_input0(this),
      m_input1(this),
      m_input2(this),
      m_output(this),
      m_stored_count() {}

AssemblerM::~AssemblerM() = default;

void AssemblerM::upgrade_anchors() {
  m_input0.m_parent = this;
  m_input1.m_parent = this;
  m_input2.m_parent = this;
  m_output.m_parent = this;
}

bool AssemblerM::is_breakable() { return true; }

std::string AssemblerM::get_name() { return "Assembler"; }

std::vector<Anchor*> AssemblerM::get_anchors() {
  return {&m_input0, &m_input1, &m_input2, &m_output};
}

void AssemblerM::draw(DrawManagerBase* draw_manager) {
  draw_manager->draw_label(m_point.x, m_point.y, "[[Assembler]]");
  draw_manager->draw_label(m_point.x + 2, m_point.y - 1, "I1");
  draw_manager->draw_label(m_point.x + 5, m_point.y - 1, "I2");
  draw_manager->draw_label(m_point.x + 8, m_point.y - 1, "I3");
  draw_manager->draw_label(m_point.x + 5, m_point.y + 1, "O");
}

void AssemblerM::create_idx(size_t* buffer, size_t self_idx) {
  for (int y = m_point.y; y < m_point.y + 1; ++y) {
    for (int x = m_point.x; x < m_point.x + 15; ++x) {
      buffer[y * 120 + x] = self_idx;
    }
  }
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

void AssemblerM::evaluate(ProductiveStats* stats,
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

constexpr size_t NULL_IDX = static_cast<size_t>(-1);

MachineManager::MachineManager()
    : m_machines(), m_sparse_idx(120 * 30), m_anchor_idx(120 * 30) {}

MachineManager::~MachineManager() = default;

void MachineManager::add_machine(std::shared_ptr<Machine> machine) {
  m_machines.push_back(machine);
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
std::shared_ptr<Machine> MachineManager::remove_machine(Point point) {
  for (size_t i = 0; i < 120 * 30; ++i) {
    m_sparse_idx[i] = NULL_IDX;
  }

  for (size_t i = 0; i < m_machines.size(); ++i) {
    std::shared_ptr<Machine> machine = m_machines[i];
    machine->create_idx(m_sparse_idx.data(), i);
  }

  size_t index_to_remove = m_sparse_idx[point.y * 120 + point.x];

  if (index_to_remove != static_cast<size_t>(-1) &&
      m_machines[index_to_remove]->is_breakable()) {
    std::shared_ptr<Machine> machine = m_machines[index_to_remove];
    m_machines.erase(m_machines.begin() + index_to_remove);
    return machine;
  }

  return nullptr;
}

void MachineManager::draw(DrawManagerBase* draw_manager) {
  for (size_t i = 0; i < m_machines.size(); ++i) {
    std::shared_ptr<Machine> machine = m_machines[i];
    machine->draw(draw_manager);
  }
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
Anchor* MachineManager::get_anchor(Point point) {
  for (size_t i = 0; i < 120 * 30; ++i) {
    m_anchor_idx[i] = nullptr;
  }

  for (size_t i = 0; i < m_machines.size(); ++i) {
    std::shared_ptr<Machine> machine = m_machines[i];
    machine->create_anchor(m_anchor_idx.data());
  }

  return m_anchor_idx[point.y * 120 + point.x];
}

void MachineManager::evaluate(ProductiveStats* stats,
                              std::default_random_engine rng) {
  for (size_t i = 0; i < m_machines.size(); ++i) {
    std::shared_ptr<Machine> machine = m_machines[i];
    machine->evaluate(stats, rng);
  }
}

}  // namespace factory_game