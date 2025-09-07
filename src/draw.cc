#include "draw.h"

namespace factory_game {

DrawManager::DrawManager(SHORT width, SHORT height, HANDLE stdout_handle)
    : m_width(width),
      m_height(height),
      m_stdout_handle(stdout_handle),
      m_current_buffer(m_width * m_height, ' '),
      m_back_buffer(m_width * m_height, ' ') {
  // カーソル位置を先頭に移動
  SetConsoleCursorPosition(m_stdout_handle, {0, 0});

  // カーソルを非表示
  CONSOLE_CURSOR_INFO cursor_info = {1, false};
  SetConsoleCursorInfo(m_stdout_handle, &cursor_info);
}

DrawManager::~DrawManager() {
  clear();
  present();

  // カーソル位置を先頭に移動
  SetConsoleCursorPosition(m_stdout_handle, {0, 0});

  // カーソルを表示
  CONSOLE_CURSOR_INFO cursor_info = {1, true};
  SetConsoleCursorInfo(m_stdout_handle, &cursor_info);
}

void DrawManager::clear() {
  fill(m_back_buffer.begin(), m_back_buffer.end(), ' ');
}

void DrawManager::draw_label(int x, int y, std::string text) {
  if (y < 0 || y >= m_height) {
    return;
  }

  for (size_t i = 0; i < text.length(); ++i) {
    size_t current_x = x + i;

    if (current_x >= m_width) {
      continue;
    }

    m_back_buffer[y * m_width + current_x] = text[i];
  }
}

void DrawManager::draw_label_box(int x, int y, std::string text) {
  draw_line_box(x - 1, y - 1, static_cast<int>(text.length() + 2), 3);
  draw_label(x, y, text);
}

void DrawManager::draw_clear_box(int x, int y, int width, int height) {
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      int current_x = x + i;
      int current_y = y + j;
      if (current_x < 0 || current_x >= m_width || current_y < 0 ||
          current_y >= m_height) {
        continue;
      }
      m_back_buffer[current_y * m_width + current_x] = ' ';
    }
  }
}

void DrawManager::draw_line_box(int x, int y, int width, int height) {
  draw_hv_line(x, y, x + width - 1, y);                            // 上辺
  draw_hv_line(x, y + height - 1, x + width - 1, y + height - 1);  // 下辺
  draw_hv_line(x, y, x, y + height - 1);                           // 左辺
  draw_hv_line(x + width - 1, y, x + width - 1, y + height - 1);   // 右辺
}

void DrawManager::draw_hv_line(int x0, int y0, int x1, int y1) {
  // 垂直線
  if (x0 == x1) {
    if (y0 > y1) std::swap(y0, y1);
    for (int y = y0; y <= y1; ++y) {
      if (x0 >= 0 && x0 < m_width && y >= 0 && y < m_height) {
        if (y == y0 || y == y1) {
          m_back_buffer[y * m_width + x0] = '+';
        } else {
          m_back_buffer[y * m_width + x0] = '|';
        }
      }
    }
  }
  // 水平線
  else if (y0 == y1) {
    if (x0 > x1) std::swap(x0, x1);
    for (int x = x0; x <= x1; ++x) {
      if (x >= 0 && x < m_width && y1 >= 0 && y1 < m_height) {
        if (x == x0 || x == x1) {
          m_back_buffer[y0 * m_width + x] = '+';
        } else {
          m_back_buffer[y0 * m_width + x] = '-';
        }
      }
    }
  }
}

void DrawManager::present() {
  // バックバッファとカレントバッファを比較し、変更点のみを描画
  for (SHORT y = 0; y < m_height; ++y) {
    for (SHORT x = 0; x < m_width; ++x) {
      size_t index = y * m_width + x;

      if (m_back_buffer[index] != m_current_buffer[index]) {
        SetConsoleCursorPosition(m_stdout_handle, {x, y});

        DWORD written;
        WriteConsole(m_stdout_handle, &m_back_buffer[index], 1, &written,
                     nullptr);
      }
    }
  }

  // 現在の画面の状態を更新するため、バックバッファの内容をカレントバッファにコピー
  copy(m_back_buffer.begin(), m_back_buffer.end(), m_current_buffer.begin());
}

}  // namespace factory_game