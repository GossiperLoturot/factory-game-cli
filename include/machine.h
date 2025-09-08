#pragma once

#include <memory>
#include <random>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <glm/vec2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "draw.h"
#include "foundation.h"

namespace factory_game {

class Pipe;     // for pointer reference

class Machine;  // for pointer reference

class Anchor {
public:
  Machine* m_parent;
  std::vector<std::shared_ptr<Pipe>> m_pipes;
  std::vector<Anchor*> m_piped_anchors;

  Anchor();
  explicit Anchor(Machine* parent);
  ~Anchor();
};

class MachineSpatialIdx {
public:
  MachineSpatialIdx(std::unordered_map<glm::ivec2, std::shared_ptr<Machine>>& spatial_idx, std::shared_ptr<Machine>& cursor);
  ~MachineSpatialIdx();

  void Write(glm::ivec2 point);

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

  virtual void upgrade_anchors() = 0;
  virtual std::vector<Anchor*> get_anchors() = 0;
  virtual void create_anchor(Anchor** buffer) = 0;

  virtual void insert_item(Item given_item) = 0;
  virtual void evaluate(EvaluateContext* stats,
                        std::default_random_engine rng) = 0;
};

class InputM : public Machine {
 public:
  InputM(glm::ivec2 point, Item item);
  ~InputM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Item item;
  Anchor m_output;
};

class OutputM : public Machine {
 public:
  OutputM(glm::ivec2 point, Item item);
  ~OutputM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Item item;
  Anchor m_input;

  int m_stored_count;
};

class ElectrolyzerM : public Machine {
 public:
  ElectrolyzerM(glm::ivec2 point);
  ~ElectrolyzerM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output0;
  Anchor m_output1;

  int m_stored_count;
};

class CutterM : public Machine {
 public:
  CutterM(glm::ivec2 point);
  ~CutterM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output;

  int m_stored_count[2];
};

class LaserM : public Machine {
 public:
  LaserM(glm::ivec2 point);
  ~LaserM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output;

  int m_stored_count;
};

class AssemblerM : public Machine {
 public:
  explicit AssemblerM(glm::ivec2 point);
  ~AssemblerM() override;

  bool is_breakable() override;

  void draw(DrawManagerBase* draw_manager) override;
  void build_spatial_idx(MachineSpatialIdx writer) override;

  void upgrade_anchors() override;
  std::vector<Anchor*> get_anchors() override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(EvaluateContext* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input0;
  Anchor m_input1;
  Anchor m_input2;
  Anchor m_output;

  int m_stored_count[3];
};

class MachineManager {
 public:
  MachineManager();
  ~MachineManager();

  void build_spatial_idx();
  void add_machine(std::shared_ptr<Machine> machine);
  void remove_machine(std::shared_ptr<Machine> machine);
  std::shared_ptr<Machine> find_machine(glm::ivec2 point);
  void draw(DrawManagerBase* draw_manager);

  Anchor* get_anchor(glm::ivec2 point);

  void evaluate(EvaluateContext* stats, std::default_random_engine rng);

 private:
  std::unordered_set<std::shared_ptr<Machine>> m_machines;
  std::unordered_map<glm::ivec2, std::shared_ptr<Machine>> m_spatial_idx;

  std::vector<Anchor*> m_anchor_idx;
};

}  // namespace factory_game