#include "draw.h"
#include "state.h"

int main() {
  // Windows
#if defined(WIN32)
  auto draw_manager = std::make_unique<DrawManagerWindows>();
#endif

  // Linux
#if defined(__linux__)
  auto draw_manager = std::make_unique<DrawManagerLinux>();
#endif

  State* state = new TitleState();

  do {
    State* new_state = state->update(*draw_manager);

    if (new_state != state) {
      delete state;
      state = new_state;
    }
  } while (state != nullptr);

  return EXIT_SUCCESS;
}
