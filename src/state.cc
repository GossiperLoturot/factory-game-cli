#include "state.h"

#include <iomanip>

#include "draw.h"

// STATE

State::State() {
}

State::~State() = default;

// TITLE STATE

TitleState::TitleState() {
}

TitleState::~TitleState() = default;

State* TitleState::update(DrawManagerBase& draw_manager) {
  draw_manager.clear();

  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "Title");

  draw_manager.present();

  draw_manager.capture_input();

  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return new InGameState{1};
  }

  if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
    return nullptr;
  }

  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
    return new InGameState{1};
  }

  return this;
}

// IN-GAME STATE

InGameState::InGameState(int stage) : m_mode{Mode::PLACE_PIPE}, m_mode_state{}, m_rng{std::random_device()()}, m_stats{} {
  m_stats.stage = stage;
  m_stats.design_time = 60 * 60;

  // Stage 1.
  if (stage == 1) {
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::WATER);
      m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{30, 25}, Item::HYDROGEN);
      m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{70, 25}, Item::OXYGEN);
      m_machine_manager.add_machine(std::move(machine));
    }
  }

  // Stage 2.
  if (stage == 2) {
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{30, 5}, Item::SILICON);
      m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::SOLDERING_IRON);
      m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{70, 5}, Item::CIRCUIT_BOARD);
      m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{50, 25}, Item::CHIP);
      m_machine_manager.add_machine(std::move(machine));
    }
  }
}

InGameState::~InGameState() = default;

State* InGameState::update(DrawManagerBase& draw_manager) {
  draw_manager.clear();

  m_pipe_manager.draw(draw_manager);
  m_machine_manager.draw(draw_manager);

  draw_manager.capture_input();

  if (draw_manager.handle_input_keycode(KEYCODE_TAB)) {
    if (m_mode == Mode::PLACE_PIPE || m_mode == Mode::LINK_PIPE) {
      m_mode = Mode::PLACE_MACHINE;
      m_mode_state.PlaceMachine = {MachineKind::ELECTROLYZER};
    } else if (m_mode == Mode::PLACE_MACHINE) {
      m_mode = Mode::PLACE_PIPE;
      m_mode_state.PlacePipe = {};
    }
  }

  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    if (m_mode != Mode::EVALUATE) {
      m_mode = Mode::EVALUATE;
      m_mode_state.Evaluate = {0};
    }
  }

  if (draw_manager.handle_input_keycode('r')) {
    if (m_mode != Mode::EVALUATE) {
      if (m_mode == Mode::RECIPE) {
        m_mode = Mode::PLACE_PIPE;
      } else {
        m_mode = Mode::RECIPE;
      }
    }
  }

  switch (m_mode) {
    case Mode::PLACE_PIPE: {
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Pipe");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        int machine_id, port_id;
        if (m_machine_manager.find_machine_port(point, machine_id, port_id)) {
          m_mode = Mode::LINK_PIPE;
          m_mode_state.LinkPipe = {point, machine_id, port_id};
        }
      }

      break;
    }
    case Mode::LINK_PIPE: {
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Link Pipe");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        // 生産ラインの入出力の関連付け
        int machine_id, port_id;
        if (m_machine_manager.find_machine_port(point, machine_id, port_id)) {
          auto pipe = std::make_unique<Pipe>(m_mode_state.LinkPipe.point, point);
          m_pipe_manager.add_pipe(std::move(pipe));

          m_mode = Mode::PLACE_PIPE;
          m_mode_state.PlacePipe = {};
        }
      }

      break;
    }
    case Mode::PLACE_MACHINE: {
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Machine");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe, Space: Change Machine");

      MachineKind& machine_kind{m_mode_state.PlaceMachine.machine};
      switch (machine_kind) {
        case MachineKind::ELECTROLYZER:
          draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Electrolyzer]");
          break;
        case MachineKind::CUTTER:
          draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Cutter]");
          break;
        case MachineKind::LAZER:
          draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Lazer]");
          break;
        case MachineKind::ASSEMBLER:
          draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Assembler]");
          break;
      }

      if (draw_manager.handle_input_keycode(KEYCODE_SPACE)) {
        switch (machine_kind) {
          case MachineKind::ELECTROLYZER:
            machine_kind = MachineKind::CUTTER;
            break;
          case MachineKind::CUTTER:
            machine_kind = MachineKind::LAZER;
            break;
          case MachineKind::LAZER:
            machine_kind = MachineKind::ASSEMBLER;
            break;
          case MachineKind::ASSEMBLER:
            machine_kind = MachineKind::ELECTROLYZER;
            break;
        }
      }

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        switch (machine_kind) {
          case MachineKind::ELECTROLYZER: {
            auto machine = std::make_unique<Electrolyzer>(point);
            m_machine_manager.add_machine(std::move(machine));
            break;
          }
          case MachineKind::CUTTER: {
            auto machine = std::make_unique<Cutter>(point);
            m_machine_manager.add_machine(std::move(machine));
            break;
          }
          case MachineKind::LAZER: {
            auto machine = std::make_unique<Laser>(point);
            m_machine_manager.add_machine(std::move(machine));
            break;
          }
          case MachineKind::ASSEMBLER: {
            auto machine = std::make_unique<Assembler>(point);
            m_machine_manager.add_machine(std::move(machine));
            break;
          }
        }
      }

      break;
    }
    case Mode::EVALUATE: {
      std::stringstream status_stream;
      status_stream << "Evaluating... : " << std::setprecision(2) << std::fixed << (static_cast<float>(m_mode_state.Evaluate.time_count) / 60.0f) << " / 3.00";
      std::string status = status_stream.str();

      draw_manager.draw_label_box(50, 1, status);

      m_mode_state.Evaluate.time_count++;
      if (m_mode_state.Evaluate.time_count <= 60 * 3) {
        // m_machine_manager.evaluate(&m_stats, m_rng);
      } else {
        return new ResultState{m_stats};
      }

      break;
    }
    case Mode::RECIPE: {
      draw_manager.draw_clear_box(20, 4, 80, 20);
      draw_manager.draw_line_box(20, 4, 80, 20);
      draw_manager.draw_label_box(21, 5, "Recipe Book : R to Exit");

      draw_manager.draw_label(22, 8, "[Electrolyzer]");
      draw_manager.draw_label(22, 9, "Input : Water");
      draw_manager.draw_label(22, 10, "Output 1 : Hydrogen");
      draw_manager.draw_label(22, 11, "Output 2 : Oxygen");

      draw_manager.draw_label(22, 13, "[Cutter]");
      draw_manager.draw_label(22, 14, "Input : Silicon");
      draw_manager.draw_label(22, 15, "Output : Silicon Wafer");

      draw_manager.draw_label(22, 17, "[Cutter]");
      draw_manager.draw_label(22, 18, "Input : Circuit Wafer");
      draw_manager.draw_label(22, 19, "Output : Circuit");

      draw_manager.draw_label(52, 8, "[Laser]");
      draw_manager.draw_label(52, 9, "Input : Silicon Wafer");
      draw_manager.draw_label(52, 10, "Output : Circuit Wafer");

      draw_manager.draw_label(52, 12, "[Assembler]");
      draw_manager.draw_label(52, 13, "Input 1 : Circuit");
      draw_manager.draw_label(52, 14, "Input 2 : Soldering Iron");
      draw_manager.draw_label(52, 15, "Input 3 : Circuit Board");
      draw_manager.draw_label(52, 16, "Output : Chip");

      break;
    }
  }

  // remove pipe or machine
  if (m_mode != Mode::EVALUATE && m_mode != Mode::RECIPE) {
    int x, y;
    if (draw_manager.handle_input_mouse(MOUSE_RCLICK, x, y)) {
      if (m_mode == Mode::LINK_PIPE) {
        m_mode = Mode::PLACE_PIPE;
        m_mode_state.PlacePipe = {};
      }

      int pipe_id;
      if (m_pipe_manager.find_pipe(glm::ivec2{x, y}, pipe_id)) {
        m_pipe_manager.remove_pipe(pipe_id);
      }

      int machine_id;
      if (m_machine_manager.find_machine(glm::ivec2{x, y}, machine_id)) {
        m_machine_manager.remove_machine(machine_id);
      }
    }
  }

  // timer
  std::stringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2) << std::setfill('0') << (m_stats.design_time % 60);
  std::string time = time_stream.str();
  draw_manager.draw_label_box(draw_manager.get_width() - 1 - static_cast<int>(time.size()), draw_manager.get_height() - 4, time);

  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "IN-GAME");

  draw_manager.present();

  if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
    return new ResultState{m_stats};
  }

  return this;
}

// RESULT STATE

ResultState::ResultState(EvaluateContext stats) : m_stats{stats} {
}

ResultState::~ResultState() = default;

// ゲームの結果標示、処理は雑
State* ResultState::update(DrawManagerBase& draw_manager) {
  draw_manager.clear();

  draw_manager.draw_label_box(30, 10, "Game Result");

  // time
  std::stringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2) << std::setfill('0') << (m_stats.design_time % 60) << " / 60:00";
  std::string time = time_stream.str();
  draw_manager.draw_label(30, 14, time);

  // score
  bool is_perfect = true;
  bool is_bad_inv = false;
  float score_value = 0.0f;
  for (int i = 0; i < m_stats.items.size(); ++i) {
    std::stringstream line_stream;
    line_stream << item_to_string(m_stats.items.at(i)) << " : " << m_stats.counts.at(i) << " unit.";
    std::string line = line_stream.str();
    draw_manager.draw_label(30, 16 + i, line);

    is_perfect &= (m_stats.counts.at(i) > 0);
    is_bad_inv |= (m_stats.counts.at(i) > 0);

    score_value += static_cast<float>(m_stats.counts.at(i));
  }
  score_value *= (static_cast<float>(m_stats.design_time) / 3600.0f);
  std::stringstream score_stream;
  score_stream << "Score : " << std::setprecision(2) << std::fixed << score_value;
  std::string score = score_stream.str();
  draw_manager.draw_label(30, 12, score);

  // grade
  if (!is_bad_inv) {
    draw_manager.draw_label(43, 10, "Bad...");
  } else if (!is_perfect) {
    draw_manager.draw_label(43, 10, "Good!");
  } else {
    draw_manager.draw_label(43, 10, "Perfect!!!");
  }

  // frame
  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "Result");

  draw_manager.present();

  draw_manager.capture_input();

  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    if (m_stats.stage == 1 && is_bad_inv) {
      return new InGameState{2};
    } else {
      return nullptr;
    }
  }

  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return nullptr;
  }

  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
    if (m_stats.stage == 1 && is_bad_inv) {
      return new InGameState{2};
    } else {
      return nullptr;
    }
  }

  return this;
}
