#include "pipe.h"

#include <algorithm>

namespace factory_game {

// SPATIAL IDX

PipeSpatialIdx::PipeSpatialIdx(
    std::unordered_map<glm::ivec2, std::shared_ptr<Pipe>>& spatial_idx,
    std::shared_ptr<Pipe>& cursor)
    : m_spatial_idx(spatial_idx), m_cursor(cursor) {}

PipeSpatialIdx::~PipeSpatialIdx() = default;

void PipeSpatialIdx::Write(const glm::ivec2 point) const {
  m_spatial_idx.insert_or_assign(point, m_cursor);
}

// PIPE

Pipe::Pipe(const glm::ivec2 begin, const glm::ivec2 end)
    : begin(begin), end(end) {}

Pipe::~Pipe() = default;

void Pipe::draw(DrawManagerBase* draw_manager) const {
  if (begin.x == end.x || begin.y == end.y) {
    draw_manager->draw_hv_line(begin.x, begin.y, end.x, end.y);
  } else {
    draw_manager->draw_hv_line(begin.x, begin.y, begin.x, end.y);
    draw_manager->draw_hv_line(begin.x, end.y, end.x, end.y);
  }
}

void Pipe::build_spatial_idx(const PipeSpatialIdx& writer) const {
  // 垂直パイプ
  if (begin.x == end.x) {
    int y0 = begin.y, y1 = end.y;
    if (y0 > y1) std::swap(y0, y1);

    const int x = begin.x;
    for (int y = y0; y <= y1; ++y) writer.Write(glm::ivec2(x, y));
  }

  // 水平パイプ
  else if (begin.y == end.y) {
    int x0 = begin.x, x1 = end.x;
    if (x0 > x1) std::swap(x0, x1);

    const int y = begin.y;
    for (int x = x0; x <= x1; ++x) writer.Write(glm::ivec2(x, y));
  }

  // L字パイプ
  else {
    // 垂直部分
    int y0 = begin.y, y1 = end.y;
    if (y0 > y1) std::swap(y0, y1);

    const int x = begin.x;
    for (int v = y0; v <= y1; ++v) writer.Write(glm::ivec2(x, v));

    // 水平部分
    int x0 = begin.x, x1 = end.x;
    if (x0 > x1) std::swap(x0, x1);

    const int y = end.y;
    for (int u = x0; u <= x1; ++u) writer.Write(glm::ivec2(u, y));
  }
}

// PIPE MANAGER

PipeManager::PipeManager() {}

PipeManager::~PipeManager() = default;

void PipeManager::add_pipe(const std::shared_ptr<Pipe>& pipe) {
  m_pipes.insert(pipe);
  build_spatial_idx();
}

void PipeManager::build_spatial_idx() {
  m_spatial_idx.clear();

  for (auto pipe : m_pipes) {
    auto writer = PipeSpatialIdx(m_spatial_idx, pipe);
    pipe->build_spatial_idx(writer);
  }
}

void PipeManager::remove_pipe(const std::shared_ptr<Pipe>& pipe) {
  m_pipes.erase(pipe);
  build_spatial_idx();
}

std::shared_ptr<Pipe> PipeManager::find_pipe(const glm::ivec2 point) {
  const auto it = m_spatial_idx.find(point);
  if (it == m_spatial_idx.end()) return nullptr;
  return it->second;
}

void PipeManager::draw(DrawManagerBase* draw_manager) const {
  for (const auto pipe : m_pipes) {
    pipe->draw(draw_manager);
  }
}

}  // namespace factory_game