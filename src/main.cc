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

  auto& state_manager_ref = *state_manager;
  while (!state_manager_ref.is_terminate()) {
    state_manager_ref.update(*draw_manager);
  }

  return EXIT_SUCCESS;
}
