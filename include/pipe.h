#pragma once

#include <memory>
#include <vector>

#include "draw.h"
#include "foundation.h"

namespace factory_game {

class Anchor;  // for pointer reference
class Pipe {
 public:
  Point begin;
  Point end;
  Anchor* begin_anchor;
  Anchor* end_anchor;

  Pipe(Point begin, Point end, Anchor* begin_anchor, Anchor* end_anchor);
  ~Pipe();

  std::string get_name();
  void draw(DrawManagerBase* draw_manager);
  void create_idx(size_t* buffer, size_t self_idx);
};

class PipeManager {
 public:
  PipeManager();
  ~PipeManager();

  void draw(DrawManagerBase* draw_manager);
  void add_pipe(std::shared_ptr<Pipe> pipe);
  void remove_pipe(std::shared_ptr<Pipe> point);
  std::shared_ptr<Pipe> remove_pipe(Point point);

 private:
  std::vector<std::shared_ptr<Pipe>> m_pipes;
  std::vector<size_t> m_field;
};

}  // namespace factory_game
