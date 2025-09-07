#pragma once

#include <memory>
#include <vector>

#include "draw.h"
#include "foundation.h"
#include "pipe.h"

namespace factory_game {

class Input {
 public:
  Point point;
  Item m_item;

  Input(Point point, Item item);
  virtual ~Input();

  virtual std::string get_name() = 0;
  virtual void draw(DrawManagerBase* draw_manager) = 0;
  virtual void create_anchor(Anchor** buffer) = 0;
};

class Output {
 public:
  Point point;
  Item m_item;

  Output(Point point, Item item);
  virtual ~Output();

  virtual std::string get_name() = 0;
  virtual void draw(DrawManagerBase* draw_manager) = 0;
  virtual void create_anchor(Anchor** buffer) = 0;
};

class InoutManager {
 public:
  InoutManager();
  ~InoutManager();

  void draw(DrawManagerBase* draw_manager);
  void add_input(std::shared_ptr<Input> input);
  void add_output(std::shared_ptr<Output> output);
  Anchor* get_anchor(Point point);

 private:
  std::vector<std::shared_ptr<Input>> m_inputs;
  std::vector<std::shared_ptr<Output>> m_outputs;
  std::vector<Anchor*> m_anchor_idx;
};

}  // namespace factory_game