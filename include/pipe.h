#pragma once

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include <glm/vec2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "draw.h"

namespace factory_game {

class Anchor;  // for pointer reference

class Pipe;     // for pointer reference

class PipeSpatialIdx {
public:
  PipeSpatialIdx(std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>>& spatial_idx, std::shared_ptr<Pipe>& cursor);
  ~PipeSpatialIdx();

  void Write(glm::ivec2 point);

private:
  std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>>& m_spatial_idx;
  std::shared_ptr<Pipe>& m_cursor;
};

class Pipe {
 public:
  glm::ivec2 begin;
  glm::ivec2 end;
  Anchor* begin_anchor;
  Anchor* end_anchor;

  Pipe(glm::ivec2 begin, glm::ivec2 end, Anchor* begin_anchor, Anchor* end_anchor);
  ~Pipe();

  void draw(DrawManagerBase* draw_manager);
  void build_spatial_idx(PipeSpatialIdx writer);
};

class PipeManager {
 public:
  PipeManager();
  ~PipeManager();

  void build_spatial_idx();
  void add_pipe(std::shared_ptr<Pipe> pipe);
  void remove_pipe(std::shared_ptr<Pipe> point);
  std::shared_ptr<Pipe> find_pipe(glm::ivec2 point);
  void draw(DrawManagerBase* draw_manager);

 private:
  std::unordered_set<std::shared_ptr<Pipe>> m_pipes;
  std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>> m_spatial_idx;
};

}  // namespace factory_game
