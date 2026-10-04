#include <cstdlib>
#include <memory>

#include "draw.h"
#include "state.h"

int main() {
  // Windows
#ifdef _WIN32
  auto draw_manager = std::make_unique<DrawManagerWindows>();
#endif

  // Linux
#ifdef __linux__
  auto draw_manager = std::make_unique<DrawManagerLinux>();
#endif

  auto state_manager = std::make_unique<StateManager>();

  while (state_manager->is_running()) {
    state_manager->update(*draw_manager);
  }

  return EXIT_SUCCESS;
}
