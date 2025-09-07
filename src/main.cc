#include "draw.h"
#include "state.h"

namespace factory_game {

int main() {
  DrawManagerBase* draw_manager = new DrawManagerWindows();
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