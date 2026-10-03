#include "draw.h"

#include <termios.h>

#include <iostream>
#include <thread>

DrawManagerBase::DrawManagerBase() = default;
DrawManagerBase::~DrawManagerBase() = default;

// Windows

#if defined(WIN32)
DrawManagerWindows::DrawManagerWindows() {
  m_width = 120;
  m_height = 30;

  m_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(m_stdout_handle, &m_out_mode);
  DWORD out_mode{m_out_mode};
  out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(m_stdout_handle, out_mode);

  m_stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(m_stdin_handle, &m_in_mode);
  DWORD in_mode{m_in_mode};
  in_mode |= ENABLE_WINDOW_INPUT;
  in_mode |= ENABLE_MOUSE_INPUT;
  in_mode |= ENABLE_EXTENDED_FLAGS;
  in_mode &= ~ENABLE_QUICK_EDIT_MODE;
  SetConsoleMode(m_stdin_handle, in_mode);

  GetConsoleCursorInfo(m_stdin_handle, &m_cursor_info);
  CONSOLE_CURSOR_INFO cursor_info = m_cursor_info;
  cursor_info.bVisible = FALSE;
  SetConsoleCursorInfo(m_stdout_handle, &cursor_info);

  m_current_buffer = std::vector(m_width * m_height, ' ');
  m_back_buffer = std::vector(m_width * m_height, ' ');

  m_input = INPUT_RECORD();
}

DrawManagerWindows::~DrawManagerWindows() {
  SetConsoleMode(m_stdout_handle, m_out_mode);
  SetConsoleMode(m_stdin_handle, m_in_mode);
  SetConsoleCursorInfo(m_stdout_handle, &m_cursor_info);
}

int DrawManagerWindows::get_width() {
  return m_width;
}

int DrawManagerWindows::get_height() {
  return m_height;
}

void DrawManagerWindows::clear() {
  std::fill(m_back_buffer.begin(), m_back_buffer.end(), ' ');
}

void DrawManagerWindows::draw_label(int x, int y, std::string_view text) {
  if (y < 0 || y >= m_height) {
    return;
  }

  for (int i = 0; i < text.length(); ++i) {
    int current_x = x + i;
    if (current_x < 0 || current_x >= m_width) {
      continue;
    }
    m_back_buffer.at(y * m_width + current_x) = text.at(i);
  }
}

void DrawManagerWindows::draw_label_box(int x, int y, std::string_view text) {
  draw_line_box(x - 1, y - 1, text.length() + 2, 3);
  draw_label(x, y, text);
}

void DrawManagerWindows::draw_clear_box(int x, int y, int width, int height) {
  for (int j = 0; j < height; ++j) {
    int current_y = y + j;
    if (current_y < 0 || current_y >= m_height) {
      continue;
    }

    for (int i = 0; i < width; ++i) {
      int current_x = x + i;
      if (current_x < 0 || current_x >= m_width) {
        continue;
      }
      m_back_buffer.at(current_y * m_width + current_x) = ' ';
    }
  }
}

void DrawManagerWindows::draw_line_box(int x, int y, int width, int height) {
  draw_hv_line(x, y, x + width - 1, y);                            // 上辺
  draw_hv_line(x, y + height - 1, x + width - 1, y + height - 1);  // 下辺
  draw_hv_line(x, y, x, y + height - 1);                           // 左辺
  draw_hv_line(x + width - 1, y, x + width - 1, y + height - 1);   // 右辺
}

void DrawManagerWindows::draw_hv_line(int x0, int y0, int x1, int y1) {
  // 垂直線
  if (x0 == x1 && x0 >= 0 && x0 < m_width) {
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    for (int y = y0; y <= y1; ++y) {
      if (y < 0 || y >= m_height) {
        continue;
      }

      if (y == y0 || y == y1) {
        m_back_buffer.at(y * m_width + x0) = '+';
      } else {
        m_back_buffer.at(y * m_width + x0) = '|';
      }
    }
  }

  // 水平線
  if (y0 == y1 && y0 >= 0 && y0 < m_height) {
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    for (int x = x0; x <= x1; ++x) {
      if (x < 0 && x >= m_width) {
        continue;
      }

      if (x == x0 || x == x1) {
        m_back_buffer.at(y0 * m_width + x) = '+';
      } else {
        m_back_buffer.at(y0 * m_width + x) = '-';
      }
    }
  }
}

void DrawManagerWindows::present() {
  // バックバッファとカレントバッファを比較し、変更点のみを描画
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int index = y * m_width + x;

      if (m_back_buffer.at(index) != m_current_buffer.at(index)) {
        COORD coord;
        coord.X = static_cast<SHORT>(x);
        coord.Y = static_cast<SHORT>(y);
        SetConsoleCursorPosition(m_stdout_handle, coord);

        DWORD written;
        WriteConsole(m_stdout_handle, &m_back_buffer.at(index), 1, &written, nullptr);

        m_current_buffer.at(index) = m_back_buffer.at(index);
      }
    }
  }

  Sleep(16);
}

void DrawManagerWindows::capture_input() {
  DWORD num_events;
  GetNumberOfConsoleInputEvents(m_stdin_handle, &num_events);

  if (num_events == 0) {
    m_input = INPUT_RECORD();
    return;
  }

  ReadConsoleInput(m_stdin_handle, &m_input, 1, &num_events);
}

bool DrawManagerWindows::handle_input_keycode(int keycode) {
  return m_input.EventType == KEY_EVENT && m_input.Event.KeyEvent.bKeyDown && m_input.Event.KeyEvent.wVirtualKeyCode == keycode;
}

bool DrawManagerWindows::handle_input_mouse(int state, int& x, int& y) {
  if (m_input.EventType == MOUSE_EVENT && m_input.Event.MouseEvent.dwEventFlags == 0 && m_input.Event.MouseEvent.dwButtonState & state) {
    x = m_input.Event.MouseEvent.dwMousePosition.X;
    y = m_input.Event.MouseEvent.dwMousePosition.Y;
    return true;
  }

  return false;
}
#endif

// Linux

#if defined(__linux__)
DrawManagerLinux::DrawManagerLinux() {
  m_width = 120;
  m_height = 30;

  tcgetattr(STDIN_FILENO, &m_terminfo);
  termios terminfo{m_terminfo};
  terminfo.c_lflag &= ~(ICANON | ECHO);
  terminfo.c_cc[VMIN] = 0;
  terminfo.c_cc[VTIME] = 0;
  tcsetattr(STDOUT_FILENO, TCSAFLUSH, &terminfo);

  std::cout << "\x1b[?1049h";
  std::cout << "\x1b[?25l";
  std::cout << "\x1b[?1000h";
  std::cout << "\x1b[?1006h";
  std::cout << std::flush;

  m_current_buffer = std::vector(m_width * m_height, ' ');
  m_back_buffer = std::vector(m_width * m_height, ' ');
}

DrawManagerLinux::~DrawManagerLinux() {
  tcsetattr(STDOUT_FILENO, TCSANOW, &m_terminfo);

  std::cout << "\x1b[?1006l";
  std::cout << "\x1b[?1000l";
  std::cout << "\x1b[?25h";
  std::cout << "\x1b[?1049l";
  std::cout << std::flush;
}

int DrawManagerLinux::get_width() {
  return m_width;
}

int DrawManagerLinux::get_height() {
  return m_height;
}

void DrawManagerLinux::clear() {
  std::fill(m_back_buffer.begin(), m_back_buffer.end(), ' ');
}

void DrawManagerLinux::draw_label(int x, int y, std::string_view text) {
  if (y < 0 || y >= m_height) {
    return;
  }

  for (int i = 0; i < text.length(); ++i) {
    int current_x = x + i;
    if (current_x < 0 || current_x >= m_width) {
      continue;
    }
    m_back_buffer.at(y * m_width + current_x) = text.at(i);
  }
}

void DrawManagerLinux::draw_label_box(int x, int y, std::string_view text) {
  draw_line_box(x - 1, y - 1, text.length() + 2, 3);
  draw_label(x, y, text);
}

void DrawManagerLinux::draw_clear_box(int x, int y, int width, int height) {
  for (int j = 0; j < height; ++j) {
    int current_y = y + j;
    if (current_y < 0 || current_y >= m_height) {
      continue;
    }

    for (int i = 0; i < width; ++i) {
      int current_x = x + i;
      if (current_x < 0 || current_x >= m_width) {
        continue;
      }
      m_back_buffer.at(current_y * m_width + current_x) = ' ';
    }
  }
}

void DrawManagerLinux::draw_line_box(int x, int y, int width, int height) {
  draw_hv_line(x, y, x + width - 1, y);                            // 上辺
  draw_hv_line(x, y + height - 1, x + width - 1, y + height - 1);  // 下辺
  draw_hv_line(x, y, x, y + height - 1);                           // 左辺
  draw_hv_line(x + width - 1, y, x + width - 1, y + height - 1);   // 右辺
}

void DrawManagerLinux::draw_hv_line(int x0, int y0, int x1, int y1) {
  // 垂直線
  if (x0 == x1 && x0 >= 0 && x0 < m_width) {
    if (y0 > y1) {
      std::swap(y0, y1);
    }

    for (int y = y0; y <= y1; ++y) {
      if (y < 0 || y >= m_height) {
        continue;
      }

      if (y == y0 || y == y1) {
        m_back_buffer.at(y * m_width + x0) = '+';
      } else {
        m_back_buffer.at(y * m_width + x0) = '|';
      }
    }
  }

  // 水平線
  if (y0 == y1 && y0 >= 0 && y0 < m_height) {
    if (x0 > x1) {
      std::swap(x0, x1);
    }

    for (int x = x0; x <= x1; ++x) {
      if (x < 0 && x >= m_width) {
        continue;
      }

      if (x == x0 || x == x1) {
        m_back_buffer.at(y0 * m_width + x) = '+';
      } else {
        m_back_buffer.at(y0 * m_width + x) = '-';
      }
    }
  }
}

void DrawManagerLinux::present() {
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int index = y * m_width + x;

      if (m_back_buffer.at(index) != m_current_buffer.at(index)) {
        std::cout << "\x1b[" << std::to_string(y + 1) << ";" << std::to_string(x + 1) << "H";
        std::cout << m_back_buffer.at(index);

        m_current_buffer.at(index) = m_back_buffer.at(index);
      }
    }
  }
  std::cout << std::flush;

  std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void DrawManagerLinux::capture_input() {
  m_input_buffer.clear();

  char buf[32];
  int n = read(STDIN_FILENO, buf, sizeof(buf));
  if (n > 0) {
    m_input_buffer.assign(buf, buf + n);
  }
}

bool DrawManagerLinux::handle_input_keycode(int keycode) {
  if (m_input_buffer.empty()) {
    return false;
  }

  if (m_input_buffer.size() == 1 && m_input_buffer.at(0) == keycode) {
    return true;
  }

  return false;
}

bool DrawManagerLinux::handle_input_mouse(int state, int& x, int& y) {
  if (m_input_buffer.empty()) {
    return false;
  }

  if (m_input_buffer.size() > 6 && m_input_buffer.at(0) == '\x1b' && m_input_buffer.at(1) == '[' && m_input_buffer.at(2) == '<') {
    int cb, cx, cy;
    char m;
    int parsed = sscanf(m_input_buffer.data(), "\x1b[<%d;%d;%d%c", &cb, &cx, &cy, &m);

    std::cout << "\x1b[1;32H";
    for (char c : m_input_buffer) {
      std::cout << c;
    }
    std::cout << std::flush;

    if (parsed == 4 && m == 'M' && cb == state) {
      x = cx - 1;
      y = cy - 1;
      return true;
    }
  }

  return false;
}
#endif
