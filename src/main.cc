#include "draw.h"
#include "state.h"

namespace factory_game {

int main() {
#if defined(WIN32)
  DrawManagerBase* draw_manager = new DrawManagerWindows();
#endif
#if defined(__linux__)
  DrawManagerBase* draw_manager = new DrawManagerLinux();
#endif
  State* state = new TitleState();

  do {
    State* new_state = state->update(draw_manager);

    if (new_state != state) {
      delete state;
      state = new_state;
    }
  } while (state != nullptr);

  return EXIT_SUCCESS;
}

}  // namespace factory_game

int main() { return factory_game::main(); }
