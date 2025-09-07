#pragma once

#include <windows.h>

#include <string>
#include <vector>

namespace factory_game {

class DrawManager {
 public:
  SHORT m_width;
  SHORT m_height;
  HANDLE m_stdout_handle;

  DrawManager(SHORT m_width, SHORT m_height, HANDLE m_stdout_handle);
  ~DrawManager();

  void clear();
  void draw_label(int x, int y, std::string text);
  void draw_label_box(int x, int y, std::string text);
  void draw_clear_box(int x, int y, int width, int height);
  void draw_line_box(int x, int y, int width, int height);
  void draw_hv_line(int x0, int y0, int x1, int y1);
  void present();

 private:
  std::vector<char> m_current_buffer;
  std::vector<char> m_back_buffer;
};

}  // namespace factory_game