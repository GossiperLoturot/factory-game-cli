#include "machine.h"

// BASE MACHINE

MachineBase::MachineBase(glm::ivec2 point) : m_point{point} {
}

// INPUT DUCT

InputDuct::InputDuct(glm::ivec2 point, Item item) : MachineBase{point}, item{item} {
}

bool InputDuct::is_breakable() {
  return false;
}

void InputDuct::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x + 2, m_point.y - 1, item_to_string(item));
  draw_manager.draw_label(m_point.x, m_point.y, "[[Input]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> InputDuct::rects() {
  return std::vector<glm::ivec4>{};
}

std::vector<glm::ivec2> InputDuct::ports() {
  glm::ivec2 port{m_point.x + 5, m_point.y + 1};
  return std::vector<glm::ivec2>{port};
}

// OUTPUT DUCT

OutputDuct::OutputDuct(glm::ivec2 point, Item item) : MachineBase{point}, item{item} {
}

bool OutputDuct::is_breakable() {
  return false;
}

void OutputDuct::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x + 2, m_point.y + 1, item_to_string(item));
  draw_manager.draw_label(m_point.x, m_point.y, "[[Output]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
}

std::vector<glm::ivec4> OutputDuct::rects() {
  return std::vector<glm::ivec4>{};
}

std::vector<glm::ivec2> OutputDuct::ports() {
  glm::ivec2 port{m_point.x + 5, m_point.y - 1};
  return std::vector<glm::ivec2>{port};
}

// ELECTROLYZER MACHINE

Electrolyzer::Electrolyzer(glm::ivec2 point) : MachineBase{point} {
}

bool Electrolyzer::is_breakable() {
  return true;
}

void Electrolyzer::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Electrolyzer]]");
  draw_manager.draw_label(m_point.x + 7, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O1");
  draw_manager.draw_label(m_point.x + 10, m_point.y + 1, "O2");
}

std::vector<glm::ivec4> Electrolyzer::rects() {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return std::vector<glm::ivec4>{bb};
}

std::vector<glm::ivec2> Electrolyzer::ports() {
  glm::ivec2 input_port{m_point.x + 7, m_point.y - 1};
  glm::ivec2 output_port0{m_point.x + 5, m_point.y + 1};
  glm::ivec2 output_port1{m_point.x + 10, m_point.y + 1};
  return std::vector<glm::ivec2>{input_port, output_port0, output_port1};
}

// CUTTER MACHINE

Cutter::Cutter(glm::ivec2 point) : MachineBase{point} {
}

bool Cutter::is_breakable() {
  return true;
}

void Cutter::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Cutter]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Cutter::rects() {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return std::vector<glm::ivec4>{bb};
}

std::vector<glm::ivec2> Cutter::ports() {
  glm::ivec2 input_port{m_point.x + 5, m_point.y - 1};
  glm::ivec2 output_port{m_point.x + 5, m_point.y + 1};
  return std::vector<glm::ivec2>{input_port, output_port};
}

// LAZER MACHINE

Laser::Laser(glm::ivec2 point) : MachineBase{point} {
}

bool Laser::is_breakable() {
  return true;
}

void Laser::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Laser]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Laser::rects() {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return std::vector<glm::ivec4>{bb};
}

std::vector<glm::ivec2> Laser::ports() {
  glm::ivec2 input_port{m_point.x + 5, m_point.y - 1};
  glm::ivec2 output_port{m_point.x + 5, m_point.y + 1};
  return std::vector<glm::ivec2>{input_port, output_port};
}

// ASSEMBLER MACHINE

Assembler::Assembler(glm::ivec2 point) : MachineBase{point} {
}

bool Assembler::is_breakable() {
  return true;
}

void Assembler::draw(DrawManagerBase& draw_manager) {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Assembler]]");
  draw_manager.draw_label(m_point.x + 2, m_point.y - 1, "I1");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I2");
  draw_manager.draw_label(m_point.x + 8, m_point.y - 1, "I3");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Assembler::rects() {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return std::vector<glm::ivec4>{bb};
}

std::vector<glm::ivec2> Assembler::ports() {
  glm::ivec2 input_port0{m_point.x + 2, m_point.y - 1};
  glm::ivec2 input_port1{m_point.x + 5, m_point.y - 1};
  glm::ivec2 input_port2{m_point.x + 8, m_point.y - 1};
  glm::ivec2 output_port{m_point.x + 5, m_point.y + 1};
  return std::vector<glm::ivec2>{input_port0, input_port1, input_port2, output_port};
}

// MACHINE MANAGER

MachineManager::MachineManager() : m_machines{}, m_spatial_idx{}, m_port_spatial_idx{} {
}

void MachineManager::build_spatial_idx() {
  m_spatial_idx.clear();
  m_port_spatial_idx.clear();

  for (int machine_id = 0; machine_id < m_machines.size(); ++machine_id) {
    const auto& machine_ptr = m_machines.at(machine_id);
    if (machine_ptr == nullptr) {
      continue;
    }
    MachineBase& machine = *machine_ptr;

    for (glm::ivec4 rect : machine.rects()) {
      for (int x = rect.x; x < rect.z; ++x) {
        for (int y = rect.y; y < rect.w; ++y) {
          m_spatial_idx.insert_or_assign(glm::ivec2{x, y}, machine_id);
        }
      }
    }

    std::vector<glm::ivec2> ports = machine.ports();
    for (int j = 0; j < ports.size(); ++j) {
      m_port_spatial_idx.insert_or_assign(ports.at(j), std::make_pair(machine_id, j));
    }
  }
}

int MachineManager::add_machine(std::unique_ptr<MachineBase> machine) {
  int machine_id = m_machines.size();
  m_machines.push_back(std::move(machine));
  build_spatial_idx();
  return machine_id;
}

MachineBase& MachineManager::get_machine(int machine_id) {
  const auto& machine_ptr = m_machines.at(machine_id);
  if (machine_ptr == nullptr) {
    throw std::runtime_error("Machine with id " + std::to_string(machine_id) + " does not exist.");
  }
  return *machine_ptr;
}

std::unique_ptr<MachineBase> MachineManager::remove_machine(int machine_id) {
  auto machine_ptr = std::move(m_machines.at(machine_id));
  build_spatial_idx();
  return machine_ptr;
}

bool MachineManager::find_machine(glm::ivec2 point, int& machine_id) {
  auto it = m_spatial_idx.find(point);
  if (it == m_spatial_idx.end()) {
    return false;
  }
  machine_id = it->second;
  return true;
}

bool MachineManager::find_machine_port(glm::ivec2 point, int& machine_id, int& port_id) {
  auto it = m_port_spatial_idx.find(point);
  if (it == m_port_spatial_idx.end()) {
    return false;
  }
  machine_id = it->second.first;
  port_id = it->second.second;
  return true;
}

void MachineManager::draw(DrawManagerBase& draw_manager) {
  for (const auto& machine_ptr : m_machines) {
    if (machine_ptr == nullptr) {
      continue;
    }
    MachineBase& machine = *machine_ptr;

    machine.draw(draw_manager);
  }
}
