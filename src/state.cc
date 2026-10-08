#include "state.h"

#include <iomanip>
#include <utility>
#include <variant>

#include "draw.h"
#include "foundation.h"
#include "machine.h"

// substate constructors

PlacePipeSubState::PlacePipeSubState() {
}

LinkPipeSubState::LinkPipeSubState(glm::ivec2 point, int machine_id, int port_id) : m_point{point}, m_machine_id{machine_id}, m_port_id{port_id} {
}

PlaceMachineSubState::PlaceMachineSubState(MachineKind machine) : m_machine{machine} {
}

EvaluateSubState::EvaluateSubState() : m_time_count{} {
}

RecipeSubState::RecipeSubState() {
}

SubmitSubState::SubmitSubState() {
}

// ::update

void remove_machine(DrawManagerBase& draw_manager, MachineManager& machine_manager, int x, int y) {
  int pipe_id;
  if (machine_manager.find_pipe(glm::ivec2{x, y}, pipe_id)) {
    Pipe pipe = machine_manager.remove_pipe(pipe_id);

    auto& port0 = machine_manager.get_machine(pipe.m_begin_machine_id)->port(pipe.m_begin_port_id);
    auto& port1 = machine_manager.get_machine(pipe.m_end_machine_id)->port(pipe.m_end_port_id);
    // unregister pipe
    std::erase(port0.m_pipe_ids, pipe_id);
    std::erase(port1.m_pipe_ids, pipe_id);
    // unregister machine port
    std::erase(port0.m_machine_port_ids, std::make_pair(pipe.m_end_machine_id, pipe.m_end_port_id));
    std::erase(port1.m_machine_port_ids, std::make_pair(pipe.m_begin_machine_id, pipe.m_begin_port_id));
  }

  int machine_id;
  if (machine_manager.find_machine(glm::ivec2{x, y}, machine_id)) {
    int n_pipe = machine_manager.get_machine(machine_id)->port_count();
    for (int port_id = 0; port_id < n_pipe; port_id++) {
      auto& port = machine_manager.get_machine(machine_id)->port(port_id);
      for (int pipe_id : port.m_pipe_ids) {
        Pipe pipe = machine_manager.remove_pipe(pipe_id);

        auto& port0 = machine_manager.get_machine(pipe.m_begin_machine_id)->port(pipe.m_begin_port_id);
        auto& port1 = machine_manager.get_machine(pipe.m_end_machine_id)->port(pipe.m_end_port_id);
        // unregister pipe
        std::erase(port0.m_pipe_ids, pipe_id);
        std::erase(port1.m_pipe_ids, pipe_id);
        // unregister machine port
        std::erase(port0.m_machine_port_ids, std::make_pair(pipe.m_end_machine_id, pipe.m_end_port_id));
        std::erase(port1.m_machine_port_ids, std::make_pair(pipe.m_begin_machine_id, pipe.m_begin_port_id));
      }
    }
    machine_manager.remove_machine(machine_id);
  }
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, PlacePipeSubState&& substate) {
  draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Pipe");
  draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

  if (draw_manager.handle_input_keycode(KEYCODE_TAB)) {
    return PlaceMachineSubState{MachineKind::ELECTROLYZER};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return EvaluateSubState{};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_R)) {
    return RecipeSubState{};
  }

  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
    glm::ivec2 point{x, y};

    int machine_id, port_id;
    if (machine_manager.find_machine_port(point, machine_id, port_id)) {
      return LinkPipeSubState{point, machine_id, port_id};
    }
  }
  if (draw_manager.handle_input_mouse(MOUSE_RCLICK, x, y)) {
    remove_machine(draw_manager, machine_manager, x, y);
  }

  eval_ctx.m_design_time -= 1;

  return std::move(substate);
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, LinkPipeSubState&& substate) {
  draw_manager.draw_label(1, draw_manager.get_height() - 2, "Link Pipe");
  draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Cancel, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

  if (draw_manager.handle_input_keycode(KEYCODE_TAB)) {
    return PlaceMachineSubState{MachineKind::ELECTROLYZER};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return EvaluateSubState{};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_R)) {
    return RecipeSubState{};
  }

  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y)) {
    glm::ivec2 point0 = substate.m_point;
    glm::ivec2 point1{x, y};

    int machine_id0 = substate.m_machine_id;
    int port_id0 = substate.m_port_id;
    int machine_id1, port_id1;
    if (machine_manager.find_machine_port(point1, machine_id1, port_id1)) {
      int pipe_id = machine_manager.add_pipe(Pipe{point0, machine_id0, port_id0, point1, machine_id1, port_id1});
      auto& port0 = machine_manager.get_machine(machine_id0)->port(port_id0);
      auto& port1 = machine_manager.get_machine(machine_id1)->port(port_id1);
      // register pipe
      port0.m_pipe_ids.push_back(pipe_id);
      port1.m_pipe_ids.push_back(pipe_id);
      // register machine port
      port0.m_machine_port_ids.push_back({machine_id1, port_id1});
      port1.m_machine_port_ids.push_back({machine_id0, port_id0});

      return PlacePipeSubState{};
    }
  }
  if (draw_manager.handle_input_mouse(MOUSE_RCLICK, x, y)) {
    return PlacePipeSubState{};
  }

  eval_ctx.m_design_time -= 1;

  return std::move(substate);
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, PlaceMachineSubState&& substate) {
  draw_manager.draw_label(1, draw_manager.get_height() - 2, "Place Machine");
  draw_manager.draw_label(1, draw_manager.get_height() - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe, Space: Change Machine");

  if (draw_manager.handle_input_keycode(KEYCODE_TAB)) {
    return PlacePipeSubState{};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return EvaluateSubState{};
  }
  if (draw_manager.handle_input_keycode(KEYCODE_R)) {
    return RecipeSubState{};
  }
  MachineKind& machine_kind = substate.m_machine;
  std::string_view machine_str = machine_to_string(machine_kind);
  draw_manager.draw_label(15, draw_manager.get_height() - 2, machine_str);
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
      machine_manager.add_machine(std::move(machine));
    } else if (machine_kind == MachineKind::CUTTER) {
      auto machine = std::make_unique<Cutter>(point);
      machine_manager.add_machine(std::move(machine));
    } else if (machine_kind == MachineKind::LAZER) {
      auto machine = std::make_unique<Laser>(point);
      machine_manager.add_machine(std::move(machine));
    } else if (machine_kind == MachineKind::ASSEMBLER) {
      auto machine = std::make_unique<Assembler>(point);
      machine_manager.add_machine(std::move(machine));
    }
  }
  if (draw_manager.handle_input_mouse(MOUSE_RCLICK, x, y)) {
    remove_machine(draw_manager, machine_manager, x, y);
  }

  eval_ctx.m_design_time -= 1;

  return std::move(substate);
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, EvaluateSubState&& substate) {
  machine_manager.evaluate(eval_ctx);

  std::stringstream status_stream{};
  float progress = static_cast<float>(substate.m_time_count) / 60.0f;
  status_stream << std::setprecision(2) << std::fixed << "Evaluating... : " << progress << " / 3.00";
  std::string status_str = status_stream.str();

  draw_manager.draw_label_box(50, 1, status_str);

  if (substate.m_time_count > 180) {
    return SubmitSubState{};
  }

  substate.m_time_count += 1;

  return std::move(substate);
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, RecipeSubState&& substate) {
  if (draw_manager.handle_input_keycode(KEYCODE_R)) {
    return PlacePipeSubState{};
  }

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

  eval_ctx.m_design_time -= 1;

  return std::move(substate);
}

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, SubmitSubState&& substate) {
  return std::move(substate);
}

// ::is_terminate

bool is_terminate(const PlacePipeSubState& substate) {
  return false;
}

bool is_terminate(const LinkPipeSubState& substate) {
  return false;
}

bool is_terminate(const PlaceMachineSubState& substate) {
  return false;
}

bool is_terminate(const EvaluateSubState& substate) {
  return false;
}

bool is_terminate(const RecipeSubState& substate) {
  return false;
}

bool is_terminate(const SubmitSubState& substate) {
  return true;
}

// state constructors

TitleState::TitleState() {
}

InGameState::InGameState() : m_machine_manager{}, m_substate{PlacePipeSubState{}}, m_eval_ctx{} {
}

ResultState::ResultState(EvaluateContext eval_ctx) : m_eval_ctx{std::move(eval_ctx)} {
}

TerminalState::TerminalState() {
}

// InGameState new

InGameState InGameState::new_lv1() {
  InGameState state{};
  state.m_eval_ctx.m_stage = 1;
  state.m_eval_ctx.m_design_time = 60 * 60;
  {
    auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::WATER);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  {
    auto machine = std::make_unique<OutputDuct>(glm::ivec2{30, 25}, Item::HYDROGEN);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  {
    auto machine = std::make_unique<OutputDuct>(glm::ivec2{70, 25}, Item::OXYGEN);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  return state;
}

InGameState InGameState::new_lv2() {
  InGameState state{};
  state.m_eval_ctx.m_stage = 2;
  state.m_eval_ctx.m_design_time = 60 * 60;
  {
    auto machine = std::make_unique<InputDuct>(glm::ivec2{30, 5}, Item::SILICON);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  {
    auto machine = std::make_unique<InputDuct>(glm::ivec2{50, 5}, Item::SOLDERING_IRON);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  {
    auto machine = std::make_unique<InputDuct>(glm::ivec2{70, 5}, Item::CIRCUIT_BOARD);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  {
    auto machine = std::make_unique<OutputDuct>(glm::ivec2{50, 25}, Item::CHIP);
    state.m_machine_manager.add_machine(std::move(machine));
  }
  return state;
}

// ::update

State update(DrawManagerBase& draw_manager, TitleState&& state) {
  draw_manager.clear();
  draw_manager.capture_input();

  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y) || draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return InGameState::new_lv1();
  }

  if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
    return TerminalState{};
  }

  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "Title");
  draw_manager.present();

  return std::move(state);
}

State update(DrawManagerBase& draw_manager, InGameState&& state) {
  draw_manager.clear();
  draw_manager.capture_input();

  state.m_machine_manager.draw(draw_manager);

  if (draw_manager.handle_input_keycode(KEYCODE_ESCAPE)) {
    EvaluateContext eval_ctx = std::move(state.m_eval_ctx);
    return ResultState{std::move(eval_ctx)};
  }

  // update substate
  auto& machine_manager = state.m_machine_manager;
  auto& eval_ctx = state.m_eval_ctx;
  auto substate = std::visit([&draw_manager, &machine_manager, &eval_ctx](auto&& state) -> SubState { return ::update(draw_manager, machine_manager, eval_ctx, std::forward<decltype(state)>(state)); }, std::move(state.m_substate));
  state.m_substate = std::move(substate);

  // submit
  bool is_terminate = std::visit([](const auto& state) -> bool { return ::is_terminate(state); }, state.m_substate);
  if (is_terminate) {
    EvaluateContext eval_ctx{std::move(state.m_eval_ctx)};
    return ResultState{std::move(eval_ctx)};
  }

  // timer
  std::stringstream time_stream{};
  int minutes = state.m_eval_ctx.m_design_time / 60;
  int seconds = state.m_eval_ctx.m_design_time % 60;
  time_stream << std::setw(2) << std::setfill('0') << "Time : " << minutes << ":" << seconds << " / 60:00";
  std::string time_str = time_stream.str();
  draw_manager.draw_label_box(draw_manager.get_width() - 1 - static_cast<int>(time_str.size()), draw_manager.get_height() - 4, time_str);

  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "IN-GAME");
  draw_manager.present();

  return std::move(state);
}

State update(DrawManagerBase& draw_manager, ResultState&& state) {
  draw_manager.clear();
  draw_manager.capture_input();

  draw_manager.draw_label_box(30, 10, "Game Result");

  // time
  std::stringstream time_stream{};
  int minutes = state.m_eval_ctx.m_design_time / 60;
  int seconds = state.m_eval_ctx.m_design_time % 60;
  time_stream << std::setw(2) << std::setfill('0') << "Time : " << minutes << ":" << seconds << " / 60:00";
  std::string time_str = time_stream.str();
  draw_manager.draw_label(30, 14, time_str);

  // score
  int line_i = 0;
  bool is_perfect = true;
  bool is_bad_inv = false;
  float score_value = 0.0f;
  for (auto [item, count] : state.m_eval_ctx.m_items) {
    std::stringstream line_stream{};
    line_stream << item_to_string(item) << " : " << count << " unit.";
    std::string line = line_stream.str();
    draw_manager.draw_label(30, 16 + line_i, line);
    line_i += 1;

    is_perfect &= (count > 0);
    is_bad_inv |= (count > 0);

    score_value += static_cast<float>(count);
  }
  score_value *= (static_cast<float>(state.m_eval_ctx.m_design_time) / 3600.0f);
  std::stringstream score_stream{};
  score_stream << std::setprecision(2) << std::fixed << "Score : " << score_value;
  std::string score_str = score_stream.str();
  draw_manager.draw_label(30, 12, score_str);

  // grade
  if (!is_bad_inv) {
    draw_manager.draw_label(43, 10, "Bad...");
  } else if (!is_perfect) {
    draw_manager.draw_label(43, 10, "Good!");
  } else {
    draw_manager.draw_label(43, 10, "Perfect!!!");
  }

  draw_manager.draw_line_box(0, 0, draw_manager.get_width(), draw_manager.get_height() - 2);
  draw_manager.draw_label_box(1, 1, "Result");
  draw_manager.present();

  // post input
  int x, y;
  if (draw_manager.handle_input_mouse(MOUSE_LCLICK, x, y) || draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    if (state.m_eval_ctx.m_stage == 1 && is_bad_inv) {
      return InGameState::new_lv2();
    } else {
      return TerminalState{};
    }
  }

  if (draw_manager.handle_input_keycode(KEYCODE_RETURN)) {
    return TerminalState{};
  }

  return std::move(state);
}

State update(DrawManagerBase& draw_manager, TerminalState&& state) {
  return std::move(state);
}

// ::is_terminate

bool is_terminate(const TitleState& state) {
  return false;
}

bool is_terminate(const InGameState& state) {
  return false;
}

bool is_terminate(const ResultState& state) {
  return false;
}

bool is_terminate(const TerminalState& state) {
  return true;
}

// StateManager

StateManager::StateManager() : m_state{TitleState{}} {
}

bool StateManager::is_terminate() {
  return std::visit([](const auto& state) -> bool { return ::is_terminate(state); }, m_state);
}

void StateManager::update(DrawManagerBase& draw_manager) {
  auto state = std::visit([&draw_manager](auto&& state) -> State { return ::update(draw_manager, std::forward<decltype(state)>(state)); }, std::move(m_state));
  m_state = std::move(state);
}
