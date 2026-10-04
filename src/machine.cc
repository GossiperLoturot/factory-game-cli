#include "machine.h"

#include <optional>
#include <utility>

// PIPE

Pipe::Pipe(glm::ivec2 begin, int begin_machine_id, int begin_port_id, glm::ivec2 end, int end_machine_id, int end_port_id) : m_begin{begin}, m_begin_machine_id{begin_machine_id}, m_begin_port_id{begin_port_id}, m_end{end}, m_end_machine_id{end_machine_id}, m_end_port_id{end_port_id} {
}

void Pipe::draw(DrawManagerBase& draw_manager) const {
  if (m_begin.x == m_end.x || m_begin.y == m_end.y) {
    draw_manager.draw_hv_line(m_begin.x, m_begin.y, m_end.x, m_end.y);
  } else {
    draw_manager.draw_hv_line(m_begin.x, m_begin.y, m_begin.x, m_end.y);
    draw_manager.draw_hv_line(m_begin.x, m_end.y, m_end.x, m_end.y);
  }
}

std::vector<glm::ivec4> Pipe::rects() const {
  // 垂直パイプ
  if (m_begin.x == m_end.x) {
    int y0 = m_begin.y, y1 = m_end.y;
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    int x = m_begin.x;
    glm::ivec4 bb{x, y0, x + 1, y1 + 1};
    return std::vector{bb};
  }

  // 水平パイプ
  else if (m_begin.y == m_end.y) {
    int x0 = m_begin.x, x1 = m_end.x;
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    int y = m_begin.y;
    glm::ivec4 bb{x0, y, x1 + 1, y + 1};
    return std::vector{bb};
  }

  // L字パイプ
  else {
    // 垂直部分
    int y0 = m_begin.y, y1 = m_end.y;
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    int x = m_begin.x;
    glm::ivec4 bby{x, y0, x + 1, y1 + 1};

    // 水平部分
    int x0 = m_begin.x, x1 = m_end.x;
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    int y = m_end.y;
    glm::ivec4 bbx{x0, y, x1 + 1, y + 1};

    return std::vector{bby, bbx};
  }
}

// MACHINE PORT

MachinePort::MachinePort(glm::ivec2 point) : m_point{point}, m_pipe_ids{}, m_machine_port_ids{} {
}

// BASE MACHINE

MachineBase::MachineBase(glm::ivec2 point) : m_point{point} {
}

MachineBase::~MachineBase() {
}

// INPUT DUCT

InputDuct::InputDuct(glm::ivec2 point, Item item) : MachineBase{point}, m_item{item}, m_ports{MachinePort{glm::ivec2{point.x + 5, point.y + 1}}} {
}

InputDuct::~InputDuct() {
}

bool InputDuct::is_breakable() {
  return false;
}

void InputDuct::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x + 2, m_point.y - 1, item_to_string(m_item));
  draw_manager.draw_label(m_point.x, m_point.y, "[[Input]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> InputDuct::rects() const {
  return {};
}

int InputDuct::port_count() const {
  return m_ports.size();
}

MachinePort& InputDuct::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& InputDuct::port(int port_id) const {
  return m_ports.at(port_id);
}

void InputDuct::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  auto& port = m_ports.at(0);
  if (!port.m_pipe_ids.empty()) {
    std::uniform_int_distribution<int> dist{0, static_cast<int>(port.m_machine_port_ids.size() - 1)};
    auto& machine_port = port.m_machine_port_ids.at(dist(ctx.m_rng));
    int machine_id = machine_port.first;
    int port_id = machine_port.second;
    mgr.get_machine(machine_id)->insert_item(port_id, m_item);
  }
}

void InputDuct::insert_item(int port_id, Item item) {
}

// OUTPUT DUCT

OutputDuct::OutputDuct(glm::ivec2 point, Item item) : MachineBase{point}, m_item{item}, m_ports{MachinePort{glm::ivec2{point.x + 5, point.y - 1}}}, m_stored_count{} {
}

OutputDuct::~OutputDuct() {
}

bool OutputDuct::is_breakable() {
  return false;
}

void OutputDuct::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x + 2, m_point.y + 1, item_to_string(m_item));
  draw_manager.draw_label(m_point.x, m_point.y, "[[Output]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
}

std::vector<glm::ivec4> OutputDuct::rects() const {
  return {};
}

int OutputDuct::port_count() const {
  return m_ports.size();
}

MachinePort& OutputDuct::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& OutputDuct::port(int port_id) const {
  return m_ports.at(port_id);
}

void OutputDuct::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  if (ctx.m_items.contains(m_item)) {
    ctx.m_items.at(m_item) += m_stored_count;
  } else {
    ctx.m_items.insert_or_assign(m_item, m_stored_count);
  }
  m_stored_count = 0;
}

void OutputDuct::insert_item(int port_id, Item item) {
  if (port_id == 0 && item == m_item) {
    m_stored_count += 1;
  }
}

// ELECTROLYZER MACHINE

Electrolyzer::Electrolyzer(glm::ivec2 point) : MachineBase{point}, m_ports{MachinePort{glm::ivec2{point.x + 7, point.y - 1}}, MachinePort{glm::ivec2{point.x + 5, point.y + 1}}, MachinePort{glm::ivec2{point.x + 10, point.y + 1}}}, m_stored_count{} {
}

Electrolyzer::~Electrolyzer() {
}

bool Electrolyzer::is_breakable() {
  return true;
}

void Electrolyzer::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Electrolyzer]]");
  draw_manager.draw_label(m_point.x + 7, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O1");
  draw_manager.draw_label(m_point.x + 10, m_point.y + 1, "O2");
}

std::vector<glm::ivec4> Electrolyzer::rects() const {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return {bb};
}

int Electrolyzer::port_count() const {
  return m_ports.size();
}

MachinePort& Electrolyzer::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& Electrolyzer::port(int port_id) const {
  return m_ports.at(port_id);
}

void Electrolyzer::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  if (m_stored_count > 0) {
    auto& port0 = m_ports.at(1);
    if (!port0.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port0.m_machine_port_ids.size() - 1)};
      auto& machine_port = port0.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::HYDROGEN);
    }

    auto& port1 = m_ports.at(2);
    if (!port1.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port1.m_machine_port_ids.size() - 1)};
      auto& machine_port = port1.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::OXYGEN);
    }

    m_stored_count -= 1;
  }
}

void Electrolyzer::insert_item(int port_id, Item item) {
  if (port_id == 0 && item == Item::WATER) {
    m_stored_count += 1;
  }
}

// CUTTER MACHINE

Cutter::Cutter(glm::ivec2 point) : MachineBase{point}, m_ports{MachinePort{glm::ivec2{point.x + 5, point.y - 1}}, MachinePort{glm::ivec2{point.x + 5, point.y + 1}}}, m_stored_count{} {
}

Cutter::~Cutter() {
}

bool Cutter::is_breakable() {
  return true;
}

void Cutter::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Cutter]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Cutter::rects() const {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return {bb};
}

int Cutter::port_count() const {
  return m_ports.size();
}

MachinePort& Cutter::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& Cutter::port(int port_id) const {
  return m_ports.at(port_id);
}

void Cutter::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  if (m_stored_count[0] > 0) {
    auto& port = m_ports.at(1);
    if (!port.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port.m_machine_port_ids.size() - 1)};
      auto& machine_port = port.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::SILICON_WAFER);
    }

    m_stored_count[0] -= 1;
  }

  if (m_stored_count[1] > 0) {
    auto& port = m_ports.at(1);
    if (!port.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port.m_machine_port_ids.size() - 1)};
      auto& machine_port = port.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::CIRCUIT);
    }

    m_stored_count[1] -= 1;
  }
}

void Cutter::insert_item(int port_id, Item item) {
  if (port_id == 0 && item == Item::SILICON) {
    m_stored_count[0] += 1;
  } else if (port_id == 0 && item == Item::CIRCUIT_WAFER) {
    m_stored_count[1] += 1;
  }
}

// LAZER MACHINE

Laser::Laser(glm::ivec2 point) : MachineBase{point}, m_ports{MachinePort{glm::ivec2{point.x + 5, point.y - 1}}, MachinePort{glm::ivec2{point.x + 5, point.y + 1}}}, m_stored_count{} {
}

Laser::~Laser() {
}

bool Laser::is_breakable() {
  return true;
}

void Laser::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Laser]]");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Laser::rects() const {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return {bb};
}

int Laser::port_count() const {
  return m_ports.size();
}

MachinePort& Laser::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& Laser::port(int port_id) const {
  return m_ports.at(port_id);
}

void Laser::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  if (m_stored_count > 0) {
    auto& port = m_ports.at(1);
    if (!port.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port.m_machine_port_ids.size() - 1)};
      auto& machine_port = port.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::CIRCUIT_WAFER);
    }

    m_stored_count -= 1;
  }
}

void Laser::insert_item(int port_id, Item item) {
  if (port_id == 0 && item == Item::SILICON_WAFER) {
    m_stored_count += 1;
  }
}

// ASSEMBLER MACHINE

Assembler::Assembler(glm::ivec2 point) : MachineBase{point}, m_ports{MachinePort{glm::ivec2{point.x + 2, point.y - 1}}, MachinePort{glm::ivec2{point.x + 5, point.y - 1}}, MachinePort{glm::ivec2{point.x + 8, point.y - 1}}, MachinePort{glm::ivec2{point.x + 5, point.y + 1}}}, m_stored_count{} {
}

Assembler::~Assembler() {
}

bool Assembler::is_breakable() {
  return true;
}

void Assembler::draw(DrawManagerBase& draw_manager) const {
  draw_manager.draw_label(m_point.x, m_point.y, "[[Assembler]]");
  draw_manager.draw_label(m_point.x + 2, m_point.y - 1, "I1");
  draw_manager.draw_label(m_point.x + 5, m_point.y - 1, "I2");
  draw_manager.draw_label(m_point.x + 8, m_point.y - 1, "I3");
  draw_manager.draw_label(m_point.x + 5, m_point.y + 1, "O");
}

std::vector<glm::ivec4> Assembler::rects() const {
  glm::ivec4 bb{m_point.x, m_point.y, m_point.x + 15, m_point.y + 1};
  return {bb};
}

int Assembler::port_count() const {
  return m_ports.size();
}

MachinePort& Assembler::port(int port_id) {
  return m_ports.at(port_id);
}

const MachinePort& Assembler::port(int port_id) const {
  return m_ports.at(port_id);
}

void Assembler::evaluate(MachineManager& mgr, EvaluateContext& ctx) {
  if (m_stored_count[0] > 0 && m_stored_count[1] > 0 && m_stored_count[2] > 0) {
    auto& port = m_ports.at(3);
    if (!port.m_pipe_ids.empty()) {
      std::uniform_int_distribution<int> dist{0, static_cast<int>(port.m_machine_port_ids.size() - 1)};
      auto& machine_port = port.m_machine_port_ids.at(dist(ctx.m_rng));
      int machine_id = machine_port.first;
      int port_id = machine_port.second;
      mgr.get_machine(machine_id)->insert_item(port_id, Item::CHIP);
    }

    m_stored_count[0] -= 1;
    m_stored_count[1] -= 1;
    m_stored_count[2] -= 1;
  }
}

void Assembler::insert_item(int port_id, Item item) {
  if (port_id != 3 && item == Item::CIRCUIT) {
    m_stored_count[0] += 1;
  } else if (port_id != 3 && item == Item::SOLDERING_IRON) {
    m_stored_count[1] += 1;
  } else if (port_id != 3 && item == Item::CIRCUIT_BOARD) {
    m_stored_count[2] += 1;
  }
}

// MACHINE MANAGER

MachineManager::MachineManager() : m_machines{}, m_machine_spatial_idx{}, m_machine_port_spatial_idx{}, m_pipes{}, m_pipe_spatial_idx{} {
}

void MachineManager::build_spatial_idx() {
  m_machine_spatial_idx.clear();
  m_machine_port_spatial_idx.clear();
  m_pipe_spatial_idx.clear();

  for (int machine_id = 0; machine_id < m_machines.size(); ++machine_id) {
    const auto& machine_ptr = m_machines.at(machine_id);
    if (machine_ptr == nullptr) {
      continue;
    }
    const MachineBase& machine = *machine_ptr;

    // register machine rects in spatial index
    for (glm::ivec4 rect : machine.rects()) {
      for (int x = rect.x; x < rect.z; ++x) {
        for (int y = rect.y; y < rect.w; ++y) {
          m_machine_spatial_idx.insert_or_assign(glm::ivec2{x, y}, machine_id);
        }
      }
    }

    // register machine ports in spatial index
    for (int port_id = 0; port_id < machine.port_count(); ++port_id) {
      auto point = machine.port(port_id).m_point;
      auto machine_port = std::make_pair(machine_id, port_id);
      m_machine_port_spatial_idx.insert_or_assign(point, machine_port);
    }
  }

  for (int pipe_id = 0; pipe_id < m_pipes.size(); ++pipe_id) {
    const auto& pipe_ptr = m_pipes.at(pipe_id);
    if (pipe_ptr == std::nullopt) {
      continue;
    }
    const Pipe& pipe = *pipe_ptr;

    // register pipe rects in spatial index
    for (glm::ivec4 rect : pipe.rects()) {
      for (int x = rect.x; x < rect.z; ++x) {
        for (int y = rect.y; y < rect.w; ++y) {
          m_pipe_spatial_idx.insert_or_assign(glm::ivec2{x, y}, pipe_id);
        }
      }
    }
  }
}

int MachineManager::add_machine(std::unique_ptr<MachineBase> machine) {
  int machine_id = m_machines.size();
  m_machines.push_back(std::move(machine));
  build_spatial_idx();
  return machine_id;
}

const std::unique_ptr<MachineBase>& MachineManager::get_machine(int machine_id) const {
  return m_machines.at(machine_id);
}

std::unique_ptr<MachineBase> MachineManager::remove_machine(int machine_id) {
  auto machine_ptr = std::exchange(m_machines.at(machine_id), nullptr);
  if (machine_ptr == nullptr) {
    throw std::runtime_error("Pipe with id " + std::to_string(machine_id) + " does not exist.");
  }
  build_spatial_idx();
  return machine_ptr;
}

bool MachineManager::find_machine(glm::ivec2 point, int& machine_id) const {
  if (m_machine_spatial_idx.contains(point)) {
    machine_id = m_machine_spatial_idx.at(point);
    return true;
  }
  return false;
}

bool MachineManager::find_machine_port(glm::ivec2 point, int& machine_id, int& port_id) const {
  if (m_machine_port_spatial_idx.contains(point)) {
    auto& kv = m_machine_port_spatial_idx.at(point);
    machine_id = kv.first;
    port_id = kv.second;
    return true;
  }
  return false;
}

int MachineManager::add_pipe(Pipe pipe) {
  int id = m_pipes.size();
  m_pipes.push_back(std::move(pipe));
  build_spatial_idx();
  return id;
}

const Pipe& MachineManager::get_pipe(int pipe_id) const {
  const auto& pipe_ptr = m_pipes.at(pipe_id);
  if (pipe_ptr == std::nullopt) {
    throw std::runtime_error("Pipe with id " + std::to_string(pipe_id) + " does not exist.");
  }
  return *pipe_ptr;
}

Pipe MachineManager::remove_pipe(int pipe_id) {
  auto pipe_ptr = std::exchange(m_pipes.at(pipe_id), std::nullopt);
  if (pipe_ptr == std::nullopt) {
    throw std::runtime_error("Pipe with id " + std::to_string(pipe_id) + " does not exist.");
  }
  build_spatial_idx();
  return std::move(*pipe_ptr);
}

bool MachineManager::find_pipe(glm::ivec2 point, int& pipe_id) const {
  if (m_pipe_spatial_idx.contains(point)) {
    pipe_id = m_pipe_spatial_idx.at(point);
    return true;
  }
  return false;
}

void MachineManager::evaluate(EvaluateContext& ctx) {
  for (const auto& machine_ptr : m_machines) {
    if (machine_ptr == nullptr) {
      continue;
    }
    MachineBase& machine = *machine_ptr;

    machine.evaluate(*this, ctx);
  }
}

void MachineManager::draw(DrawManagerBase& draw_manager) const {
  for (const auto& machine_ptr : m_machines) {
    if (machine_ptr == nullptr) {
      continue;
    }
    const MachineBase& machine = *machine_ptr;

    machine.draw(draw_manager);
  }

  for (const auto& pipe_ptr : m_pipes) {
    if (pipe_ptr == std::nullopt) {
      continue;
    }
    const Pipe& pipe = *pipe_ptr;

    pipe.draw(draw_manager);
  }
}
