#include "state.h"

#include <iomanip>

#include "draw.h"

PlacePipeSubState::PlacePipeSubState() {
}

LinkPipeSubState::LinkPipeSubState(glm::ivec2 point, int machine_id, int port_id) : m_point{point}, m_machine_id{machine_id}, m_port_id{port_id} {
}

PlaceMachineSubState::PlaceMachineSubState(MachineKind machine) : m_machine{machine} {
}

EvaluateSubState::EvaluateSubState() : m_time_count{0} {
}

RecipeSubState::RecipeSubState() {
}

TitleState::TitleState() {
}

InGameState::InGameState() : m_pipe_manager{}, m_machine_manager{}, m_sub_state{PlacePipeSubState{}}, m_rng{std::random_device{}()}, m_stats{} {
}

ResultState::ResultState(EvaluateContext stats) : m_stats{stats} {
}

TerminalState::TerminalState() {
}

InGameState generate_level(int stage) {
  InGameState level{};

  level.m_stats.stage = stage;
  level.m_stats.design_time = 60 * 60;

  // Stage 1.
  if (stage == 1) {
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::WATER);
      level.m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{30, 25}, Item::HYDROGEN);
      level.m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{70, 25}, Item::OXYGEN);
      level.m_machine_manager.add_machine(std::move(machine));
    }
  }

  // Stage 2.
  if (stage == 2) {
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{30, 5}, Item::SILICON);
      level.m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::SOLDERING_IRON);
      level.m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<InputDuct>(glm::ivec2{70, 5}, Item::CIRCUIT_BOARD);
      level.m_machine_manager.add_machine(std::move(machine));
    }
    {
      auto machine = std::make_unique<OutputDuct>(glm::ivec2{50, 25}, Item::CHIP);
      level.m_machine_manager.add_machine(std::move(machine));
    }
  }

  return level;
}

StateManager::StateManager() : m_state{TitleState{}} {
}

bool StateManager::is_running() {
  return !std::holds_alternative<TerminalState>(m_state);
}

void StateManager::update(DrawManagerBase& draw_manager) {
  if (auto state = std::get_if<TitleState>(&m_state)) {  // タイトル画面の状態
    draw_manager.clear();

    draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
    draw_manager.draw_label_box(1, 1, "Title");

    draw_manager.present();

    draw_manager.capture_input();

    int x, y;
    if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y) || draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
      m_state.emplace<InGameState>(generate_level(1));
    }

    if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
      m_state.emplace<TerminalState>();
    }
  } else if (auto state = std::get_if<InGameState>(&m_state)) {  // ゲームプレイ中の状態
    draw_manager.clear();

    state->m_pipe_manager.draw(draw_manager);
    state->m_machine_manager.draw(draw_manager);

    // # handling input

    draw_manager.capture_input();

    if (draw_manager.handle_input_keycode(KEYCODE_TAB)) {
      if (std::holds_alternative<PlacePipeSubState>(state->m_sub_state)) {
        state->m_sub_state.emplace<PlaceMachineSubState>(MachineKind::ELECTROLYZER);
      } else if (std::holds_alternative<LinkPipeSubState>(state->m_sub_state)) {
        state->m_sub_state.emplace<PlaceMachineSubState>(MachineKind::ELECTROLYZER);
      } else if (std::holds_alternative<PlaceMachineSubState>(state->m_sub_state)) {
        state->m_sub_state.emplace<PlacePipeSubState>();
      }
    }

    if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
      if (!std::holds_alternative<EvaluateSubState>(state->m_sub_state)) {
        state->m_sub_state.emplace<EvaluateSubState>();
      }
    }

    if (draw_manager.handle_input_keycode('r')) {
      if (!std::holds_alternative<EvaluateSubState>(state->m_sub_state)) {
        if (!std::holds_alternative<RecipeSubState>(state->m_sub_state)) {
          state->m_sub_state.emplace<RecipeSubState>();
        } else {
          state->m_sub_state.emplace<PlacePipeSubState>();
        }
      }
    }

    // # mode handling

    if (auto sub_state = std::get_if<PlacePipeSubState>(&state->m_sub_state)) {  // ## パイプの配置モード
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Pipe");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        int machine_id, port_id;
        if (state->m_machine_manager.find_machine_port(point, machine_id, port_id)) {
          state->m_sub_state.emplace<LinkPipeSubState>(point, machine_id, port_id);
        }
      }
    } else if (auto sub_state = std::get_if<LinkPipeSubState>(&state->m_sub_state)) {  // ## 生産ラインの入出力の関連付け
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Link Pipe");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        int machine_id, port_id;
        if (state->m_machine_manager.find_machine_port(point, machine_id, port_id)) {
          auto pipe = std::make_unique<Pipe>(sub_state->m_point, point);
          state->m_pipe_manager.add_pipe(std::move(pipe));

          state->m_sub_state.emplace<PlacePipeSubState>();
        }
      }
    } else if (auto sub_state = std::get_if<PlaceMachineSubState>(&state->m_sub_state)) {  // ## 生産ラインの機械の配置
      draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Machine");
      draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe, Space: Change Machine");

      MachineKind& machine_kind = sub_state->m_machine;
      if (machine_kind == MachineKind::ELECTROLYZER) {
        draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Electrolyzer]");
      } else if (machine_kind == MachineKind::CUTTER) {
        draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Cutter]");
      } else if (machine_kind == MachineKind::LAZER) {
        draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Lazer]");
      } else if (machine_kind == MachineKind::ASSEMBLER) {
        draw_manager.draw_label(15, draw_manager.get_height() - 2, "[Assembler]");
      }

      if (draw_manager.handle_input_keycode(KEYCODE_SPACE)) {
        if (machine_kind == MachineKind::ELECTROLYZER) {
          machine_kind = MachineKind::CUTTER;
        } else if (machine_kind == MachineKind::CUTTER) {
          machine_kind = MachineKind::LAZER;
        } else if (machine_kind == MachineKind::LAZER) {
          machine_kind = MachineKind::ASSEMBLER;
        } else if (machine_kind == MachineKind::ASSEMBLER) {
          machine_kind = MachineKind::ELECTROLYZER;
        }
      }

      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
        glm::ivec2 point{x, y};

        if (machine_kind == MachineKind::ELECTROLYZER) {
          auto machine = std::make_unique<Electrolyzer>(point);
          state->m_machine_manager.add_machine(std::move(machine));
        } else if (machine_kind == MachineKind::CUTTER) {
          auto machine = std::make_unique<Cutter>(point);
          state->m_machine_manager.add_machine(std::move(machine));
        } else if (machine_kind == MachineKind::LAZER) {
          auto machine = std::make_unique<Laser>(point);
          state->m_machine_manager.add_machine(std::move(machine));
        } else if (machine_kind == MachineKind::ASSEMBLER) {
          auto machine = std::make_unique<Assembler>(point);
          state->m_machine_manager.add_machine(std::move(machine));
        }
      }
    } else if (auto sub_state = std::get_if<EvaluateSubState>(&state->m_sub_state)) {  // ## 生産ラインの評価モード
      std::stringstream status_stream;
      status_stream << "Evaluating... : " << std::setprecision(2) << std::fixed << (sub_state->m_time_count / 60.0f) << " / 3.00";
      std::string status = status_stream.str();

      draw_manager.draw_label_box(50, 1, status);

      sub_state->m_time_count++;
      if (sub_state->m_time_count <= 60 * 3) {
        // m_machine_manager.evaluate(&m_stats, m_rng);
      } else {
        m_state.emplace<ResultState>(state->m_stats);
      }
    } else if (auto sub_state = std::get_if<RecipeSubState>(&state->m_sub_state)) {  // ## レシピの確認
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
    }

    // # remove pipe or machine

    if (!std::holds_alternative<EvaluateSubState>(state->m_sub_state) && std::holds_alternative<RecipeSubState>(state->m_sub_state)) {
      int x, y;
      if (draw_manager.handle_input_mouse(MOUSE_RCLICK, x, y)) {
        if (std::holds_alternative<LinkPipeSubState>(state->m_sub_state)) {
          state->m_sub_state.emplace<PlacePipeSubState>();
        }

        int pipe_id;
        if (state->m_pipe_manager.find_pipe(glm::ivec2{x, y}, pipe_id)) {
          state->m_pipe_manager.remove_pipe(pipe_id);
        }

        int machine_id;
        if (state->m_machine_manager.find_machine(glm::ivec2{x, y}, machine_id)) {
          state->m_machine_manager.remove_machine(machine_id);
        }
      }
    }

    // timer
    std::stringstream time_stream;
    time_stream << "Time : " << (state->m_stats.design_time / 60) << ":" << std::setw(2) << std::setfill('0') << (state->m_stats.design_time % 60);
    std::string time = time_stream.str();
    draw_manager.draw_label_box(draw_manager.get_width() - 1 - static_cast<int>(time.size()), draw_manager.get_height() - 4, time);

    draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
    draw_manager.draw_label_box(1, 1, "IN-GAME");

    draw_manager.present();

    if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
      m_state.emplace<ResultState>(state->m_stats);
    }
  } else if (auto state = std::get_if<ResultState>(&m_state)) {  // 結果画面
    draw_manager.clear();

    draw_manager.draw_label_box(30, 10, "Game Result");

    // time
    std::stringstream time_stream;
    time_stream << "Time : " << (state->m_stats.design_time / 60) << ":" << std::setw(2) << std::setfill('0') << (state->m_stats.design_time % 60) << " / 60:00";
    std::string time = time_stream.str();
    draw_manager.draw_label(30, 14, time);

    // score
    bool is_perfect = true;
    bool is_bad_inv = false;
    float score_value = 0.0f;
    for (int i = 0; i < state->m_stats.items.size(); ++i) {
      std::stringstream line_stream;
      line_stream << item_to_string(state->m_stats.items.at(i)) << " : " << state->m_stats.counts.at(i) << " unit.";
      std::string line = line_stream.str();
      draw_manager.draw_label(30, 16 + i, line);

      is_perfect &= (state->m_stats.counts.at(i) > 0);
      is_bad_inv |= (state->m_stats.counts.at(i) > 0);

      score_value += static_cast<float>(state->m_stats.counts.at(i));
    }
    score_value *= (static_cast<float>(state->m_stats.design_time) / 3600.0f);
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

    int x, y;
    if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y) || draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
      if (state->m_stats.stage == 1 && is_bad_inv) {
        m_state.emplace<InGameState>(generate_level(2));
      } else {
        m_state.emplace<TerminalState>();
      }
    }

    if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
      m_state.emplace<TerminalState>();
    }
  }
}
