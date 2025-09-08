#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "draw.h"

namespace factory_game {

class Pipe;  // for pointer reference

class PipeSpatialIdx {
 public:
  PipeSpatialIdx(
      std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>>& spatial_idx,
      std::shared_ptr<Pipe>& cursor);
  ~PipeSpatialIdx();

  void Write(glm::ivec2 point) const;

 private:
  std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>>& m_spatial_idx;
  std::shared_ptr<Pipe>& m_cursor;
};

class Pipe {
 public:
  glm::ivec2 begin;
  glm::ivec2 end;

  Pipe(glm::ivec2 begin, glm::ivec2 end);
  ~Pipe();

  void draw(DrawManagerBase* draw_manager) const;
  void build_spatial_idx(const PipeSpatialIdx& writer) const;
};

class PipeManager {
 public:
  PipeManager();
  ~PipeManager();

  void build_spatial_idx();
  void add_pipe(const std::shared_ptr<Pipe>& pipe);
  void remove_pipe(const std::shared_ptr<Pipe>& point);
  std::shared_ptr<Pipe> find_pipe(glm::ivec2 point);
  void draw(DrawManagerBase* draw_manager) const;

 private:
  std::unordered_set<std::shared_ptr<Pipe>> m_pipes;
  std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>> m_spatial_idx;
};

}  // namespace factory_game
