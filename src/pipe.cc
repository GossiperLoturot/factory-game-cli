#include "pipe.h"

#include <algorithm>

namespace factory_game {

Pipe::Pipe(Point begin, Point end, Anchor* begin_anchor, Anchor* end_anchor)
    : begin(begin),
      end(end),
      begin_anchor(begin_anchor),
      end_anchor(end_anchor) {}

Pipe::~Pipe() = default;

std::string Pipe::get_name() { return "Pipe"; }

void Pipe::draw(DrawManagerBase* draw_manager) {
  if (begin.x == end.x || begin.y == end.y) {
    draw_manager->draw_hv_line(begin.x, begin.y, end.x, end.y);
  } else {
    draw_manager->draw_hv_line(begin.x, begin.y, begin.x, end.y);
    draw_manager->draw_hv_line(begin.x, end.y, end.x, end.y);
  }
}

void Pipe::create_idx(size_t* buffer, size_t self_idx) {
  // 垂直パイプ
  if (begin.x == end.x) {
    int x = begin.x;
    int y0 = begin.y, y1 = end.y;
    if (y0 > y1) std::swap(y0, y1);
    for (int y = y0; y <= y1; ++y) buffer[y * 120 + x] = self_idx;
  }
  // 水平パイプ
  else if (begin.y == end.y) {
    int y = begin.y;
    int x0 = begin.x, x1 = end.x;
    if (x0 > x1) std::swap(x0, x1);
    for (int x = x0; x <= x1; ++x) buffer[y * 120 + x] = self_idx;
  }
  // L字パイプ
  else {
    // 垂直部分
    int x_v = begin.x;
    int y0_v = begin.y, y1_v = end.y;
    if (y0_v > y1_v) std::swap(y0_v, y1_v);
    for (int y = y0_v; y <= y1_v; ++y) buffer[y * 120 + x_v] = self_idx;
    // 水平部分
    int y_h = end.y;
    int x0_h = begin.x, x1_h = end.x;
    if (x0_h > x1_h) std::swap(x0_h, x1_h);
    for (int x = x0_h; x <= x1_h; ++x) buffer[y_h * 120 + x] = self_idx;
  }
}

// PIPE MANAGER

constexpr size_t NULL_IDX = static_cast<size_t>(-1);

PipeManager::PipeManager() : m_pipes(), m_field(120 * 30) {}

PipeManager::~PipeManager() = default;

void PipeManager::add_pipe(std::shared_ptr<Pipe> pipe) {
  m_pipes.push_back(pipe);
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
std::shared_ptr<Pipe> PipeManager::remove_pipe(Point point) {
  for (size_t i = 0; i < 120 * 30; ++i) {
    m_field[i] = NULL_IDX;
  }

  for (size_t i = 0; i < m_pipes.size(); ++i) {
    std::shared_ptr<Pipe> pipe = m_pipes[i];
    pipe->create_idx(m_field.data(), i);
  }

  size_t index_to_remove = m_field[point.y * 120 + point.x];

  if (index_to_remove != static_cast<size_t>(-1)) {
    std::shared_ptr<Pipe> pipe = m_pipes[index_to_remove];
    m_pipes.erase(m_pipes.begin() + index_to_remove);
    return pipe;
  }

  return nullptr;
}

void PipeManager::remove_pipe(std::shared_ptr<Pipe> pipe) {
  auto it = std::find(m_pipes.begin(), m_pipes.end(), pipe);
  if (it != m_pipes.end()) {
    m_pipes.erase(it);
  }
}

void PipeManager::draw(DrawManagerBase* draw_manager) {
  for (size_t i = 0; i < m_pipes.size(); ++i) {
    std::shared_ptr<Pipe> pipe = m_pipes[i];
    pipe->draw(draw_manager);
  }
}

}  // namespace factory_game