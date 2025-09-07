#pragma once

#include <memory>
#include <random>
#include <vector>

#include "draw.h"
#include "foundation.h"

namespace factory_game {

struct ProductiveStats {
  int stage;
  int design_time;
  std::vector<Item> items;
  std::vector<int> counts;
};

enum Machines {
  MACHINE_ELECTROLYZER,
  MACHINE_CUTTER,
  MACHINE_LAZER,
  MACHINE_ASSEMBLER,
};

class Machine;  // for pointer reference
class Pipe;     // for pointer reference
class Anchor {
 public:
  Machine* m_parent;
  std::vector<std::shared_ptr<Pipe>> m_pipes;
  std::vector<Anchor*> m_piped_anchors;

  Anchor();
  explicit Anchor(Machine* parent);
  ~Anchor();
};

class Machine {
 public:
  Point m_point;

  explicit Machine(Point point);
  virtual ~Machine();

  virtual void upgrade_anchors() = 0;

  virtual bool is_breakable() = 0;
  virtual std::string get_name() = 0;
  virtual std::vector<Anchor*> get_anchors() = 0;

  virtual void draw(DrawManager* draw_manager) = 0;
  virtual void create_idx(size_t* buffer, size_t self_idx) = 0;
  virtual void create_anchor(Anchor** buffer) = 0;

  virtual void insert_item(Item given_item) = 0;
  virtual void evaluate(ProductiveStats* stats,
                        std::default_random_engine rng) = 0;
};

class InputM : public Machine {
 public:
  InputM(Point point, Item item);
  ~InputM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
                std::default_random_engine rng) override;

 private:
  Item item;
  Anchor m_output;
};

class OutputM : public Machine {
 public:
  OutputM(Point point, Item item);
  ~OutputM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
                std::default_random_engine rng) override;

 private:
  Item item;
  Anchor m_input;

  int m_stored_count;
};

class ElectrolyzerM : public Machine {
 public:
  explicit ElectrolyzerM(Point point);
  ~ElectrolyzerM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output0;
  Anchor m_output1;

  int m_stored_count;
};

class CutterM : public Machine {
 public:
  explicit CutterM(Point point);
  ~CutterM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output;

  int m_stored_count[2];
};

class LaserM : public Machine {
 public:
  explicit LaserM(Point point);
  ~LaserM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
                std::default_random_engine rng) override;

 private:
  Anchor m_input;
  Anchor m_output;

  int m_stored_count;
};

class AssemblerM : public Machine {
 public:
  explicit AssemblerM(Point point);
  ~AssemblerM() override;

  void upgrade_anchors() override;

  bool is_breakable() override;
  std::string get_name() override;
  std::vector<Anchor*> get_anchors() override;

  void draw(DrawManager* draw_manager) override;
  void create_idx(size_t* buffer, size_t self_idx) override;
  void create_anchor(Anchor** buffer) override;

  void insert_item(Item given_item) override;
  void evaluate(ProductiveStats* stats,
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

  void draw(DrawManager* draw_manager);
  void add_machine(std::shared_ptr<Machine> machine);
  std::shared_ptr<Machine> remove_machine(Point point);

  Anchor* get_anchor(Point point);

  void evaluate(ProductiveStats* stats, std::default_random_engine rng);

 private:
  std::vector<std::shared_ptr<Machine>> m_machines;
  std::vector<size_t> m_sparse_idx;
  std::vector<Anchor*> m_anchor_idx;
};

}  // namespace factory_game