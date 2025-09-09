#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "draw.h"
#include "foundation.h"

namespace factory_game {

class Machine;  // for pointer reference

class MachineSpatialIdx {
 public:
  MachineSpatialIdx(
      std::unordered_map<glm::ivec2, std::shared_ptr<Machine>>& spatial_idx,
      std::shared_ptr<Machine>& cursor);
  ~MachineSpatialIdx();

  void Write(glm::ivec2 point) const;

 private:
  std::unordered_map<glm::ivec2, std::shared_ptr<Machine>>& m_spatial_idx;
  std::shared_ptr<Machine>& m_cursor;
};

class Machine {
 public:
  glm::ivec2 m_point;

  Machine(glm::ivec2 point);
  virtual ~Machine();

  virtual bool is_breakable() = 0;

  virtual void draw(DrawManagerBase* draw_manager) = 0;
  virtual void build_spatial_idx(MachineSpatialIdx writer) = 0;
};

class InputDuct : public Machine {
 public:
  InputDuct(glm::ivec2 point, Item item);
  ~InputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

 private:
  Item item;
};

class OutputDuct : public Machine {
 public:
  OutputDuct(glm::ivec2 point, Item item);
  ~OutputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

 private:
  Item item;
};

class Electrolyzer : public Machine {
 public:
  Electrolyzer(glm::ivec2 point);
  ~Electrolyzer() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;
};

class Cutter : public Machine {
 public:
  Cutter(glm::ivec2 point);
  ~Cutter() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;
};

class Laser : public Machine {
 public:
  Laser(glm::ivec2 point);
  ~Laser() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;
};

class Assembler : public Machine {
 public:
  explicit Assembler(glm::ivec2 point);
  ~Assembler() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;
};

class MachineManager {
 public:
  MachineManager();
  ~MachineManager();

  void build_spatial_idx();
  void add_machine(const std::shared_ptr<Machine>& machine);
  void remove_machine(const std::shared_ptr<Machine>& machine);
  std::shared_ptr<Machine> find_machine(glm::ivec2 point);
  void draw(DrawManagerBase* draw_manager) const;

 private:
  std::unordered_set<std::shared_ptr<Machine>> m_machines;
  std::unordered_map<glm::ivec2, std::shared_ptr<Machine>> m_spatial_idx;
};

}  // namespace factory_game
