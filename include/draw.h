#pragma once

#include <string>
#include <vector>

#include "windows.h"

namespace factory_game {

class DrawManagerBase {
 public:
  DrawManagerBase() = default;
  virtual ~DrawManagerBase() = default;

  virtual int get_width() = 0;
  virtual int get_height() = 0;

  virtual void clear() = 0;
  virtual void draw_label(int x, int y, std::string_view text) = 0;
  virtual void draw_label_box(int x, int y, std::string_view text) = 0;
  virtual void draw_clear_box(int x, int y, int width, int height) = 0;
  virtual void draw_line_box(int x, int y, int width, int height) = 0;
  virtual void draw_hv_line(int x0, int y0, int x1, int y1) = 0;
  virtual void present() = 0;

  virtual void capture_input() = 0;
  virtual bool handle_input_keycode(int keycode) = 0;
  virtual bool handle_input_mouse(int state, int& x, int& y) = 0;
};

class DrawManagerWindows : public DrawManagerBase {
 public:
  DrawManagerWindows();
  ~DrawManagerWindows() override;

  int get_width() override;
  int get_height() override;

  void clear() override;
  void draw_label(int x, int y, std::string_view text) override;
  void draw_label_box(int x, int y, std::string_view text) override;
  void draw_clear_box(int x, int y, int width, int height) override;
  void draw_line_box(int x, int y, int width, int height) override;
  void draw_hv_line(int x0, int y0, int x1, int y1) override;
  void present() override;

  void capture_input() override;
  bool handle_input_keycode(int keycode) override;
  bool handle_input_mouse(int state, int& x, int& y) override;

 private:
  int m_width;
  int m_height;
  HANDLE m_stdout_handle;
  HANDLE m_stdin_handle;
  DWORD m_out_mode;
  DWORD m_in_mode;
  CONSOLE_CURSOR_INFO m_cursor_info;
  std::vector<char> m_current_buffer;
  std::vector<char> m_back_buffer;
  INPUT_RECORD m_input;
};

}  // namespace factory_game