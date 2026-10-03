#ifndef _PIPE_H
#define _PIPE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

#include "draw.h"

class Pipe {
 public:
  glm::ivec2 begin;
  glm::ivec2 end;

  Pipe(glm::ivec2 begin, glm::ivec2 end);
  ~Pipe();

  std::vector<glm::ivec4> rects();
  void draw(DrawManagerBase& draw_manager);
};

class PipeManager {
 public:
  PipeManager();
  ~PipeManager();

  void build_spatial_idx();
  int add_pipe(std::unique_ptr<Pipe> pipe);
  std::unique_ptr<Pipe> remove_pipe(int pipe_id);
  Pipe& get_pipe(int pipe_id);
  bool find_pipe(glm::ivec2 point, int& pipe_id);
  void draw(DrawManagerBase& draw_manager);

 private:
  std::vector<std::unique_ptr<Pipe>> m_pipes;
  std::unordered_map<glm::ivec2, int> m_spatial_idx;
};

#endif  // _PIPE_H
