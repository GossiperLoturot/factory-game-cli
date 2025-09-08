#include "state.h"

#include <iomanip>

namespace factory_game {

// STATE

State::State() {}

State::~State() = default;

// TITLE STATE

TitleState::TitleState() {}

TitleState::~TitleState() = default;

State* TitleState::update(DrawManagerBase* draw_manager) {
  draw_manager->clear();

  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, "Title");

  draw_manager->present();

  draw_manager->capture_input();

  if (draw_manager->handle_input_keycode(VK_RETURN)) {
    return new InGameState(1);
  }

  if (draw_manager->handle_input_keycode(VK_ESCAPE)) {
    return nullptr;
  }

  int x, y;
  if (draw_manager->handle_input_mouse(FROM_LEFT_1ST_BUTTON_PRESSED, x, y)) {
    return new InGameState(1);
  }

  return this;
}

// IN-GAME STATE

InGameState::InGameState(const int stage)
    : m_mode(MODE_PLACE_PIPE),
      m_mode_state({}),
      m_rng(std::random_device()()),
      m_stats() {
  m_stats.stage = stage;
  m_stats.design_time = 60 * 60;

  // Stage 1.
  if (stage == 1) {
    {
      const auto machine =
          std::make_shared<InputDuct>(InputDuct(glm::ivec2(50, 5), ITEM_WATER));
      m_machine_manager.add_machine(machine);
    }
    {
      const auto machine = std::make_shared<OutputDuct>(
          OutputDuct(glm::ivec2(30, 25), ITEM_HYDROGEN));
      m_machine_manager.add_machine(machine);
    }
    {
      const auto machine = std::make_shared<OutputDuct>(
          OutputDuct(glm::ivec2(70, 25), ITEM_OXYGEN));
      m_machine_manager.add_machine(machine);
    }
  }

  // Stage 2.
  if (stage == 2) {
    {
      const auto machine = std::make_shared<InputDuct>(
          InputDuct(glm::ivec2(30, 5), ITEM_SILICON));
      m_machine_manager.add_machine(machine);
    }
    {
      const auto machine = std::make_shared<InputDuct>(
          InputDuct(glm::ivec2(50, 5), ITEM_SOLDERING_IRON));
      m_machine_manager.add_machine(machine);
    }
    {
      const auto machine = std::make_shared<InputDuct>(
          InputDuct(glm::ivec2(70, 5), ITEM_CIRCUIT_BOARD));
      m_machine_manager.add_machine(machine);
    }
    {
      const auto machine = std::make_shared<OutputDuct>(
          OutputDuct(glm::ivec2(50, 25), ITEM_CHIP));
      m_machine_manager.add_machine(machine);
    }
  }
}

InGameState::~InGameState() = default;

State* InGameState::update(DrawManagerBase* draw_manager) {
  draw_manager->clear();

  m_pipe_manager.draw(draw_manager);
  m_machine_manager.draw(draw_manager);

  draw_manager->capture_input();

  if (draw_manager->handle_input_keycode(VK_TAB)) {
    if (m_mode == MODE_PLACE_PIPE || m_mode == MODE_LINK_PIPE) {
      m_mode = MODE_PLACE_MACHINE;
      m_mode_state.PlaceMachine = {MACHINE_ELECTROLYZER};
    } else if (m_mode == MODE_PLACE_MACHINE) {
      m_mode = MODE_PLACE_PIPE;
      m_mode_state.PlacePipe = {};
    }
  }

  if (draw_manager->handle_input_keycode(VK_RETURN)) {
    if (m_mode != MODE_EVALUATE) {
      m_mode = MODE_EVALUATE;
      m_mode_state.Evaluate = {0};
    }
  }

  if (draw_manager->handle_input_keycode('R')) {
    if (m_mode != MODE_EVALUATE) {
      if (m_mode == MODE_RECIPE)
        m_mode = MODE_PLACE_PIPE;
      else if (m_mode != MODE_RECIPE)
        m_mode = MODE_RECIPE;
    }
  }

  switch (m_mode) {
    case MODE_PLACE_PIPE: {
      draw_manager->draw_label(1, draw_manager->get_height() - 2, "Place Pipe");
      draw_manager->draw_label(1, draw_manager->get_height() - 1,
                               "LClick: Place, RClick: Remove, Tab: Change "
                               "Mode, Enter: Submit, Esc: Quit, R: Recipe");

      int x, y;
      if (draw_manager->handle_input_mouse(FROM_LEFT_1ST_BUTTON_PRESSED, x,
                                           y)) {
        const auto point = glm::ivec2(x, y);

        // TODO
      }
      break;
    }
    case MODE_LINK_PIPE: {
      draw_manager->draw_label(1, draw_manager->get_height() - 2, "Link Pipe");
      draw_manager->draw_label(1, draw_manager->get_height() - 1,
                               "LClick: Place, RClick: Remove, Tab: Change "
                               "Mode, Enter: Submit, Esc: Quit, R: Recipe");

      draw_manager->draw_label(m_mode_state.LinkPipe.x, m_mode_state.LinkPipe.y,
                               "X");

      int x, y;
      if (draw_manager->handle_input_mouse(FROM_LEFT_1ST_BUTTON_PRESSED, x,
                                           y)) {
        const auto point = glm::ivec2(x, y);

        // TODO
      }
      break;
    }
    case MODE_PLACE_MACHINE: {
      draw_manager->draw_label(1, draw_manager->get_height() - 2,
                               "Place Machine");
      draw_manager->draw_label(
          1, draw_manager->get_height() - 1,
          "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, "
          "Esc: Quit, R: Recipe, Space: Change Machine");

      if (m_mode_state.PlaceMachine.machine == MACHINE_ELECTROLYZER) {
        draw_manager->draw_label(15, draw_manager->get_height() - 2,
                                 "[Electrolyzer]");
      } else if (m_mode_state.PlaceMachine.machine == MACHINE_CUTTER) {
        draw_manager->draw_label(15, draw_manager->get_height() - 2,
                                 "[Cutter]");
      } else if (m_mode_state.PlaceMachine.machine == MACHINE_LAZER) {
        draw_manager->draw_label(15, draw_manager->get_height() - 2, "[Lazer]");
      } else if (m_mode_state.PlaceMachine.machine == MACHINE_ASSEMBLER) {
        draw_manager->draw_label(15, draw_manager->get_height() - 2,
                                 "[Assembler]");
      }

      if (draw_manager->handle_input_keycode(VK_SPACE)) {
        if (m_mode_state.PlaceMachine.machine == MACHINE_ELECTROLYZER) {
          m_mode_state.PlaceMachine.machine = MACHINE_CUTTER;
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_CUTTER) {
          m_mode_state.PlaceMachine.machine = MACHINE_LAZER;
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_LAZER) {
          m_mode_state.PlaceMachine.machine = MACHINE_ASSEMBLER;
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_ASSEMBLER) {
          m_mode_state.PlaceMachine.machine = MACHINE_ELECTROLYZER;
        }
      }

      int x, y;
      if (draw_manager->handle_input_mouse(FROM_LEFT_1ST_BUTTON_PRESSED, x,
                                           y)) {
        const auto point = glm::ivec2(x, y);

        if (m_mode_state.PlaceMachine.machine == MACHINE_ELECTROLYZER) {
          const auto machine =
              std::make_shared<Electrolyzer>(Electrolyzer(point));
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_CUTTER) {
          const auto machine = std::make_shared<Cutter>(Cutter(point));
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_LAZER) {
          const auto machine = std::make_shared<Laser>(Laser(point));
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_ASSEMBLER) {
          const auto machine = std::make_shared<Assembler>(Assembler(point));
          m_machine_manager.add_machine(machine);
        }
      }

      break;
    }
  }

  if (m_mode != MODE_EVALUATE && m_mode != MODE_RECIPE) {
    int x, y;
    if (draw_manager->handle_input_mouse(RIGHTMOST_BUTTON_PRESSED, x, y)) {
      if (m_mode == MODE_LINK_PIPE) {
        m_mode = MODE_PLACE_PIPE;
        m_mode_state.PlacePipe = {};
      }

      // TODO: delete machine or pipe
    }
  }

  if (m_mode == MODE_EVALUATE) {
    std::ostringstream status_stream;
    status_stream << "Evaluating... : " << std::setprecision(2) << std::fixed
                  << (static_cast<float>(m_mode_state.Evaluate.time_count) /
                      60.0f)
                  << " / 3.00";
    std::string status = status_stream.str();

    draw_manager->draw_label_box(50, 1, status);

    m_mode_state.Evaluate.time_count++;
    if (m_mode_state.Evaluate.time_count <= 60 * 3) {
      // m_machine_manager.evaluate(&m_stats, m_rng);
    } else {
      return new ResultState(m_stats);
    }
  }

  if (m_mode == MODE_RECIPE) {
    draw_manager->draw_clear_box(20, 4, 80, 20);
    draw_manager->draw_line_box(20, 4, 80, 20);
    draw_manager->draw_label_box(21, 5, "Recipe Book : R to Exit");

    draw_manager->draw_label(22, 8, "[Electrolyzer]");
    draw_manager->draw_label(22, 9, "Input : Water");
    draw_manager->draw_label(22, 10, "Output 1 : Hydrogen");
    draw_manager->draw_label(22, 11, "Output 2 : Oxygen");

    draw_manager->draw_label(22, 13, "[Cutter]");
    draw_manager->draw_label(22, 14, "Input : Silicon");
    draw_manager->draw_label(22, 15, "Output : Silicon Wafer");

    draw_manager->draw_label(22, 17, "[Cutter]");
    draw_manager->draw_label(22, 18, "Input : Circuit Wafer");
    draw_manager->draw_label(22, 19, "Output : Circuit");

    draw_manager->draw_label(52, 8, "[Laser]");
    draw_manager->draw_label(52, 9, "Input : Silicon Wafer");
    draw_manager->draw_label(52, 10, "Output : Circuit Wafer");

    draw_manager->draw_label(52, 12, "[Assembler]");
    draw_manager->draw_label(52, 13, "Input 1 : Circuit");
    draw_manager->draw_label(52, 14, "Input 2 : Soldering Iron");
    draw_manager->draw_label(52, 15, "Input 3 : Circuit Board");
    draw_manager->draw_label(52, 16, "Output : Chip");
  }

  // timer
  std::ostringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2)
              << std::setfill('0') << (m_stats.design_time % 60);
  std::string time = time_stream.str();
  draw_manager->draw_label_box(
      draw_manager->get_width() - 1 - static_cast<int>(time.size()),
      draw_manager->get_height() - 4, time);

  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, "IN-GAME");

  draw_manager->present();

  if (draw_manager->handle_input_keycode(VK_ESCAPE)) {
    return new ResultState(m_stats);
  }

  return this;
}

// RESULT STATE

ResultState::ResultState(const EvaluateContext stats) : m_stats(stats) {}

ResultState::~ResultState() = default;

// ゲームの結果標示、処理は雑
State* ResultState::update(DrawManagerBase* draw_manager) {
  draw_manager->clear();

  draw_manager->draw_label_box(30, 10, "Game Result");

  // time
  std::ostringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2)
              << std::setfill('0') << (m_stats.design_time % 60) << " / 60:00";
  std::string time = time_stream.str();
  draw_manager->draw_label(30, 14, time);

  // score
  bool is_perfect = true;
  bool is_bad_inv = false;
  float score_value = 0.0f;
  for (size_t i = 0; i < m_stats.items.size(); ++i) {
    std::ostringstream line_stream;
    line_stream << item_to_string(m_stats.items[i]) << " : "
                << m_stats.counts[i] << " unit.";
    std::string line = line_stream.str();
    draw_manager->draw_label(30, 16 + static_cast<SHORT>(i), line);

    is_perfect &= (m_stats.counts[i] > 0);
    is_bad_inv |= (m_stats.counts[i] > 0);

    score_value += static_cast<float>(m_stats.counts[i]);
  }
  score_value *= (static_cast<float>(m_stats.design_time) / 3600.0f);
  std::ostringstream score_stream;
  score_stream << "Score : " << std::setprecision(2) << std::fixed
               << score_value;
  std::string score = score_stream.str();
  draw_manager->draw_label(30, 12, score);

  // grade
  if (!is_bad_inv)
    draw_manager->draw_label(43, 10, "Bad...");
  else if (!is_perfect)
    draw_manager->draw_label(43, 10, "Good!");
  else
    draw_manager->draw_label(43, 10, "Perfect!!!");

  // frame
  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, "Result");

  draw_manager->present();

  draw_manager->capture_input();

  if (draw_manager->handle_input_keycode(VK_RETURN)) {
    if (m_stats.stage == 1 && is_bad_inv)
      return new InGameState(2);
    else
      return nullptr;
  }

  if (draw_manager->handle_input_keycode(VK_ESCAPE)) {
    return nullptr;
  }

  int x, y;
  if (draw_manager->handle_input_mouse(FROM_LEFT_1ST_BUTTON_PRESSED, x, y)) {
    if (m_stats.stage == 1 && is_bad_inv)
      return new InGameState(2);
    else
      return nullptr;
  }

  return this;
}

}  // namespace factory_game