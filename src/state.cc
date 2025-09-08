#include "state.h"

#include <algorithm>
#include <iomanip>

namespace factory_game {

// TITLE STATE

TitleState::TitleState() = default;

TitleState::~TitleState() = default;

std::string TitleState::get_name() { return "Title"; }

State* TitleState::update(DrawManagerBase* draw_manager) {
  draw_manager->clear();

  draw_manager->draw_label(
      10, 5,
      "Anata ha seisan rainn no sekkei wo ukewou kaisya no syainn desu.");
  draw_manager->draw_label(
      10, 6,
      "Tugi tugi to maikomu irai ni taisite seigen zikan nai ni saiteki na "
      "seisan rainn wo sekkei site kudasai.");
  draw_manager->draw_label(10, 7,
                           "koku ikkoku to semaru zikan no naka de saiteki kai "
                           "wo syunji ni mitibiki dasou!");

  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, get_name());
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

InGameState::InGameState(int stage)
    : State::State(),
      m_pipe_manager(),
      m_machine_manager(),
      m_mode(MODE_PLACE_PIPE),
      m_mode_state({}),
      m_rng(std::random_device()()),
      m_stats() {
  m_stats.stage = stage;
  m_stats.design_time = 60 * 60;

  // Stage 1.
  if (stage == 1) {
    {
      auto machine = std::make_shared<InputM>(InputM(glm::ivec2(50, 5), ITEM_WATER));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
    {
      auto machine =
          std::make_shared<OutputM>(OutputM(glm::ivec2(30, 25), ITEM_HYDROGEN));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
    {
      auto machine =
          std::make_shared<OutputM>(OutputM(glm::ivec2(70, 25), ITEM_OXYGEN));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
  }

  // Stage 2.
  if (stage == 2) {
    {
      auto machine =
          std::make_shared<InputM>(InputM(glm::ivec2(30, 5), ITEM_SILICON));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
    {
      auto machine =
          std::make_shared<InputM>(InputM(glm::ivec2(50, 5), ITEM_SOLDERING_IRON));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
    {
      auto machine =
          std::make_shared<InputM>(InputM(glm::ivec2(70, 5), ITEM_CIRCUIT_BOARD));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
    {
      auto machine =
          std::make_shared<OutputM>(OutputM(glm::ivec2(50, 25), ITEM_CHIP));
      machine->upgrade_anchors();
      m_machine_manager.add_machine(machine);
    }
  }
}

InGameState::~InGameState() = default;

std::string InGameState::get_name() { return "In Game"; }

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
        Anchor* anchor = m_machine_manager.get_anchor(glm::ivec2(x, y));

        if (anchor != nullptr) {
          m_mode = MODE_LINK_PIPE;
          m_mode_state.LinkPipe = {x, y, anchor};
        }
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
        glm::ivec2 point = glm::ivec2(x, y);

        Anchor* anchor = m_machine_manager.get_anchor(point);
        if (anchor != nullptr) {
          // 生産ラインのの入出力の関連付け
          Anchor* begin_anchor = m_mode_state.LinkPipe.anchor;
          Anchor* end_anchor = anchor;
          std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>(
              Pipe(glm::ivec2(m_mode_state.LinkPipe.x, m_mode_state.LinkPipe.y),
                   point, begin_anchor, end_anchor));
          m_pipe_manager.add_pipe(pipe);
          begin_anchor->m_piped_anchors.push_back(end_anchor);
          begin_anchor->m_pipes.push_back(pipe);
          end_anchor->m_piped_anchors.push_back(begin_anchor);
          end_anchor->m_pipes.push_back(pipe);

          m_mode = MODE_PLACE_PIPE;
          m_mode_state.PlacePipe = {};
        }
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
        glm::ivec2 point = glm::ivec2(x, y);

        if (m_mode_state.PlaceMachine.machine == MACHINE_ELECTROLYZER) {
          std::shared_ptr<Machine> machine =
              std::make_shared<ElectrolyzerM>(ElectrolyzerM(point));
          machine->upgrade_anchors();
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_CUTTER) {
          std::shared_ptr<Machine> machine =
              std::make_shared<CutterM>(CutterM(point));
          machine->upgrade_anchors();
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_LAZER) {
          std::shared_ptr<Machine> machine =
              std::make_shared<LaserM>(LaserM(point));
          machine->upgrade_anchors();
          m_machine_manager.add_machine(machine);
        } else if (m_mode_state.PlaceMachine.machine == MACHINE_ASSEMBLER) {
          std::shared_ptr<Machine> machine =
              std::make_shared<AssemblerM>(AssemblerM(point));
          machine->upgrade_anchors();
          m_machine_manager.add_machine(machine);
        }
      }
      break;
    }
    default: {
      break;
    };
  }
  if (m_mode != MODE_EVALUATE && m_mode != MODE_RECIPE) {
    int x, y;
    if (draw_manager->handle_input_mouse(RIGHTMOST_BUTTON_PRESSED, x, y)) {
      if (m_mode == MODE_LINK_PIPE) {
        m_mode = MODE_PLACE_PIPE;
        m_mode_state.PlacePipe = {};
      }

      std::shared_ptr<Pipe> pipe;
      std::shared_ptr<Machine> machine;

      pipe = m_pipe_manager.find_pipe(glm::ivec2(x, y));
      if (pipe != nullptr) {
        m_pipe_manager.remove_pipe(pipe);
        Anchor* begin = pipe->begin_anchor;
        Anchor* end = pipe->end_anchor;
        {
          std::vector<std::shared_ptr<Pipe>> pipes = begin->m_pipes;
          pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
          std::vector<Anchor*> anchors = begin->m_piped_anchors;
          anchors.erase(remove(anchors.begin(), anchors.end(), end),
                        anchors.end());
        }
        {
          std::vector<std::shared_ptr<Pipe>> pipes = end->m_pipes;
          pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
          std::vector<Anchor*> anchors = end->m_piped_anchors;
          anchors.erase(remove(anchors.begin(), anchors.end(), begin),
                        anchors.end());
        }

        goto OUTSIDE;
      }
      machine = m_machine_manager.find_machine(glm::ivec2(x, y));
      if (machine != nullptr) {
        m_machine_manager.remove_machine(machine);
        std::vector<Anchor*> machine_anchors = machine->get_anchors();
        for (size_t i = 0; i < machine_anchors.size(); ++i) {
          Anchor* machine_anchor = machine_anchors[i];
          for (size_t j = 0; j < machine_anchor->m_pipes.size(); ++j) {
            std::shared_ptr<Pipe> child_pipe = machine_anchor->m_pipes[j];
            m_pipe_manager.remove_pipe(child_pipe);

            if (child_pipe != nullptr) {
              Anchor* begin = child_pipe->begin_anchor;
              Anchor* end = child_pipe->end_anchor;
              {
                std::vector<std::shared_ptr<Pipe>> pipes = begin->m_pipes;
                pipes.erase(remove(pipes.begin(), pipes.end(), pipe),
                            pipes.end());
                std::vector<Anchor*> anchors = begin->m_piped_anchors;
                anchors.erase(remove(anchors.begin(), anchors.end(), end),
                              anchors.end());
              }
              {
                std::vector<std::shared_ptr<Pipe>> pipes = end->m_pipes;
                pipes.erase(remove(pipes.begin(), pipes.end(), pipe),
                            pipes.end());
                std::vector<Anchor*> anchors = end->m_piped_anchors;
                anchors.erase(remove(anchors.begin(), anchors.end(), begin),
                              anchors.end());
              }
            }
          }
        }

        goto OUTSIDE;
      }
    OUTSIDE:;
    }

    --m_stats.design_time;
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
      m_machine_manager.evaluate(&m_stats, m_rng);
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

  std::ostringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2)
              << std::setfill('0') << (m_stats.design_time % 60);
  std::string time = time_stream.str();
  draw_manager->draw_label_box(
      draw_manager->get_width() - 1 - static_cast<int>(time.size()),
      draw_manager->get_height() - 4, time);

  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, get_name());
  draw_manager->present();

  if (draw_manager->handle_input_keycode(VK_ESCAPE)) {
    return new ResultState(m_stats);
  }

  return this;
}

// RESULT STATE

ResultState::ResultState(EvaluateContext stats)
    : State::State(), m_stats(stats) {}

ResultState::~ResultState() = default;

std::string ResultState::get_name() { return "Result"; }

// ゲームの結果標示、処理は雑
State* ResultState::update(DrawManagerBase* draw_manager) {
  draw_manager->clear();

  draw_manager->draw_label_box(30, 10, "Game Result");

  std::ostringstream time_stream;
  time_stream << "Time : " << (m_stats.design_time / 60) << ":" << std::setw(2)
              << std::setfill('0') << (m_stats.design_time % 60) << " / 60:00";
  std::string time = time_stream.str();
  draw_manager->draw_label(30, 14, time);

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

  if (!is_bad_inv)
    draw_manager->draw_label(43, 10, "Bad...");
  else if (!is_perfect)
    draw_manager->draw_label(43, 10, "Good!");
  else
    draw_manager->draw_label(43, 10, "Perfect!!!");

  draw_manager->draw_line_box(0, 0, draw_manager->get_width(),
                              draw_manager->get_height() - 2);
  draw_manager->draw_label_box(1, 1, get_name());
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