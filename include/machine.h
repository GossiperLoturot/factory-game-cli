#ifndef MACHINE_H
#define MACHINE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <optional>
#include <unordered_map>

#include "draw.h"
#include "foundation.h"

class MachineManager;

class Pipe {
 public:
  glm::ivec2 m_begin;
  int m_begin_machine_id;
  int m_begin_port_id;
  glm::ivec2 m_end;
  int m_end_machine_id;
  int m_end_port_id;

  Pipe(glm::ivec2 begin, int begin_machine_id, int begin_port_id, glm::ivec2 end, int end_machine_id, int end_port_id);

  std::vector<glm::ivec4> rects() const;
  void draw(DrawManagerBase& draw_manager) const;
};

class MachinePort {
 public:
  glm::ivec2 m_point;
  std::vector<int> m_pipe_ids;
  std::vector<std::pair<int, int>> m_machine_port_ids;

  MachinePort(glm::ivec2 point);
};

class MachineBase {
 public:
  glm::ivec2 m_point;

  MachineBase(glm::ivec2 point);
  virtual ~MachineBase();

  virtual bool is_breakable() = 0;

  virtual void draw(DrawManagerBase& draw_manager) const = 0;
  virtual std::vector<glm::ivec4> rects() const = 0;
  virtual int port_count() const = 0;
  virtual MachinePort& port(int port_id) = 0;
  virtual const MachinePort& port(int port_id) const = 0;
  virtual void evaluate(MachineManager& mgr, EvaluateContext& ctx) = 0;
  virtual void insert_item(int port_id, Item item) = 0;
};

class InputDuct : public MachineBase {
 public:
  InputDuct(glm::ivec2 point, Item item);
  ~InputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  Item m_item;
  std::array<MachinePort, 1> m_ports;
};

class OutputDuct : public MachineBase {
 public:
  OutputDuct(glm::ivec2 point, Item item);
  ~OutputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  Item m_item;
  std::array<MachinePort, 1> m_ports;
  int m_stored_count;
};

class Electrolyzer : public MachineBase {
 public:
  Electrolyzer(glm::ivec2 point);
  ~Electrolyzer() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  std::array<MachinePort, 3> m_ports;
  int m_stored_count;
};

class Cutter : public MachineBase {
 public:
  Cutter(glm::ivec2 point);
  ~Cutter() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  std::array<MachinePort, 2> m_ports;
};

class Laser : public MachineBase {
 public:
  Laser(glm::ivec2 point);
  ~Laser() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  std::array<MachinePort, 2> m_ports;
};

class Assembler : public MachineBase {
 public:
  Assembler(glm::ivec2 point);
  ~Assembler() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) const override;
  std::vector<glm::ivec4> rects() const override;
  int port_count() const override;
  MachinePort& port(int port_id) override;
  const MachinePort& port(int port_id) const override;
  void evaluate(MachineManager& mgr, EvaluateContext& ctx) override;
  void insert_item(int port_id, Item item) override;

 private:
  std::array<MachinePort, 4> m_ports;
};

class MachineManager {
 public:
  MachineManager();

  void build_spatial_idx();

  int add_machine(std::unique_ptr<MachineBase> machine);
  std::unique_ptr<MachineBase> remove_machine(int machine_id);
  const std::unique_ptr<MachineBase>& get_machine(int machine_id) const;
  bool find_machine(glm::ivec2 point, int& machine_id) const;
  bool find_machine_port(glm::ivec2 point, int& machine_id, int& port_id) const;

  int add_pipe(Pipe pipe);
  Pipe remove_pipe(int pipe_id);
  const Pipe& get_pipe(int pipe_id) const;
  bool find_pipe(glm::ivec2 point, int& pipe_id) const;

  void evaluate(EvaluateContext& ctx);

  void draw(DrawManagerBase& draw_manager) const;

 private:
  std::vector<std::unique_ptr<MachineBase>> m_machines;
  std::unordered_map<glm::ivec2, int> m_machine_spatial_idx;
  std::unordered_map<glm::ivec2, std::pair<int, int>> m_machine_port_spatial_idx;

  std::vector<std::optional<Pipe>> m_pipes;
  std::unordered_map<glm::ivec2, int> m_pipe_spatial_idx;
};

#endif  // MACHINE_H
