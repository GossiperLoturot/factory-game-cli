#include "pipe.h"

#include <memory>

// PIPE

Pipe::Pipe(glm::ivec2 begin, glm::ivec2 end) : begin{begin}, end{end} {
}

void Pipe::draw(DrawManagerBase& draw_manager) {
  if (begin.x == end.x || begin.y == end.y) {
    draw_manager.draw_hv_line(begin.x, begin.y, end.x, end.y);
  } else {
    draw_manager.draw_hv_line(begin.x, begin.y, begin.x, end.y);
    draw_manager.draw_hv_line(begin.x, end.y, end.x, end.y);
  }
}

std::vector<glm::ivec4> Pipe::rects() {
  // 垂直パイプ
  if (begin.x == end.x) {
    int y0 = begin.y, y1 = end.y;
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    int x = begin.x;
    glm::ivec4 bb{x, y0, x + 1, y1 + 1};
    return std::vector{bb};
  }

  // 水平パイプ
  else if (begin.y == end.y) {
    int x0 = begin.x, x1 = end.x;
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    int y = begin.y;
    glm::ivec4 bb{x0, y, x1 + 1, y + 1};
    return std::vector{bb};
  }

  // L字パイプ
  else {
    // 垂直部分
    int y0 = begin.y, y1 = end.y;
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    int x = begin.x;
    glm::ivec4 bby{x, y0, x + 1, y1 + 1};

    // 水平部分
    int x0 = begin.x, x1 = end.x;
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    int y = end.y;
    glm::ivec4 bbx{x0, y, x1 + 1, y + 1};

    return std::vector{bby, bbx};
  }
}

// PIPE MANAGER

PipeManager::PipeManager() : m_pipes{}, m_spatial_idx{} {
}

void PipeManager::build_spatial_idx() {
  m_spatial_idx.clear();

  for (int pipe_id = 0; pipe_id < m_pipes.size(); ++pipe_id) {
    const auto& pipe_ptr = m_pipes.at(pipe_id);
    if (pipe_ptr == nullptr) {
      continue;
    }
    Pipe& pipe = *pipe_ptr;

    for (glm::ivec4 rect : pipe.rects()) {
      for (int x = rect.x; x < rect.z; ++x) {
        for (int y = rect.y; y < rect.w; ++y) {
          m_spatial_idx.insert_or_assign(glm::ivec2{x, y}, pipe_id);
        }
      }
    }
  }
}

int PipeManager::add_pipe(std::unique_ptr<Pipe> pipe) {
  int id = m_pipes.size();
  m_pipes.push_back(std::move(pipe));
  build_spatial_idx();
  return id;
}

Pipe& PipeManager::get_pipe(int pipe_id) {
  const auto& pipe_ptr = m_pipes.at(pipe_id);
  if (pipe_ptr == nullptr) {
    throw std::runtime_error("Pipe with id " + std::to_string(pipe_id) + " does not exist.");
  }
  return *pipe_ptr;
}

std::unique_ptr<Pipe> PipeManager::remove_pipe(int pipe_id) {
  auto pipe_ptr = std::move(m_pipes.at(pipe_id));
  build_spatial_idx();
  return pipe_ptr;
}

bool PipeManager::find_pipe(glm::ivec2 point, int& pipe_id) {
  auto it = m_spatial_idx.find(point);
  if (it == m_spatial_idx.end()) {
    return false;
  }
  pipe_id = it->second;
  return true;
}

void PipeManager::draw(DrawManagerBase& draw_manager) {
  for (const auto& pipe_ptr : m_pipes) {
    if (pipe_ptr == nullptr) {
      continue;
    }
    Pipe& pipe = *pipe_ptr;

    pipe.draw(draw_manager);
  }
}
