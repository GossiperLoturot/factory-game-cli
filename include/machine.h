#ifndef _MACHINE_H
#define _MACHINE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

#include "draw.h"
#include "foundation.h"

class MachineBase {
 public:
  glm::ivec2 m_point;

  MachineBase(glm::ivec2 point);
  virtual ~MachineBase();

  virtual bool is_breakable() = 0;

  virtual void draw(DrawManagerBase& draw_manager) = 0;
  virtual std::vector<glm::ivec4> rects() = 0;
  virtual std::vector<glm::ivec2> ports() = 0;
};

class InputDuct : public MachineBase {
 public:
  InputDuct(glm::ivec2 point, Item item);
  ~InputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;

 private:
  Item item;
};

class OutputDuct : public MachineBase {
 public:
  OutputDuct(glm::ivec2 point, Item item);
  ~OutputDuct() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;

 private:
  Item item;
};

class Electrolyzer : public MachineBase {
 public:
  Electrolyzer(glm::ivec2 point);
  ~Electrolyzer() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;
};

class Cutter : public MachineBase {
 public:
  Cutter(glm::ivec2 point);
  ~Cutter() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;
};

class Laser : public MachineBase {
 public:
  Laser(glm::ivec2 point);
  ~Laser() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;
};

class Assembler : public MachineBase {
 public:
  explicit Assembler(glm::ivec2 point);
  ~Assembler() override;

  bool is_breakable() override;

  void draw(DrawManagerBase& draw_manager) override;
  std::vector<glm::ivec4> rects() override;
  std::vector<glm::ivec2> ports() override;
};

class MachineManager {
 public:
  MachineManager();
  ~MachineManager();

  void build_spatial_idx();
  int add_machine(std::unique_ptr<MachineBase> machine);
  std::unique_ptr<MachineBase> remove_machine(int machine_id);
  MachineBase& get_machine(int machine_id);
  bool find_machine(glm::ivec2 point, int& machine_id);
  bool find_machine_port(glm::ivec2 point, int& machine_id, int& port_id);
  void draw(DrawManagerBase& draw_manager);

 private:
  std::vector<std::unique_ptr<MachineBase>> m_machines;
  std::unordered_map<glm::ivec2, int> m_spatial_idx;
  std::unordered_map<glm::ivec2, std::pair<int, int>> m_port_spatial_idx;
};

#endif  // _MACHINE_H
