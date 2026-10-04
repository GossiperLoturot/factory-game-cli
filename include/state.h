#ifndef STATE_H
#define STATE_H

#include <variant>

#include "draw.h"
#include "machine.h"

enum class MachineKind {
  ELECTROLYZER,
  CUTTER,
  LAZER,
  ASSEMBLER,
};

class PlacePipeSubState {
 public:
  PlacePipeSubState();
  PlacePipeSubState(const PlacePipeSubState&) = delete;
  PlacePipeSubState& operator=(const PlacePipeSubState&) = delete;
  PlacePipeSubState(PlacePipeSubState&&) = default;
  PlacePipeSubState& operator=(PlacePipeSubState&&) = default;
};

class LinkPipeSubState {
 public:
  LinkPipeSubState(glm::ivec2 point, int machine_id, int port_id);
  LinkPipeSubState(const LinkPipeSubState&) = delete;
  LinkPipeSubState& operator=(const LinkPipeSubState&) = delete;
  LinkPipeSubState(LinkPipeSubState&&) = default;
  LinkPipeSubState& operator=(LinkPipeSubState&&) = default;

  glm::ivec2 m_point;
  int m_machine_id;
  int m_port_id;
};

class PlaceMachineSubState {
 public:
  PlaceMachineSubState(MachineKind machine);
  PlaceMachineSubState(const PlaceMachineSubState&) = delete;
  PlaceMachineSubState& operator=(const PlaceMachineSubState&) = delete;
  PlaceMachineSubState(PlaceMachineSubState&&) = default;
  PlaceMachineSubState& operator=(PlaceMachineSubState&&) = default;

  MachineKind m_machine;
};

class EvaluateSubState {
 public:
  EvaluateSubState();
  EvaluateSubState(const EvaluateSubState&) = delete;
  EvaluateSubState& operator=(const EvaluateSubState&) = delete;
  EvaluateSubState(EvaluateSubState&&) = default;
  EvaluateSubState& operator=(EvaluateSubState&&) = default;

  int m_time_count;
};

class RecipeSubState {
 public:
  RecipeSubState();
  RecipeSubState(const RecipeSubState&) = delete;
  RecipeSubState& operator=(const RecipeSubState&) = delete;
  RecipeSubState(RecipeSubState&&) = default;
  RecipeSubState& operator=(RecipeSubState&&) = default;
};

using SubState = std::variant<PlacePipeSubState, LinkPipeSubState, PlaceMachineSubState, EvaluateSubState, RecipeSubState>;

class TitleState {
 public:
  TitleState();
  TitleState(const TitleState&) = delete;
  TitleState& operator=(const TitleState&) = delete;
  TitleState(TitleState&&) = default;
  TitleState& operator=(TitleState&&) = default;
};

class InGameState {
 public:
  InGameState();
  InGameState(const InGameState&) = delete;
  InGameState& operator=(const InGameState&) = delete;
  InGameState(InGameState&&) = default;
  InGameState& operator=(InGameState&&) = default;

  MachineManager m_machine_manager;
  SubState m_sub_state;
  EvaluateContext m_eval_ctx;
};

class ResultState {
 public:
  ResultState(EvaluateContext eval_ctx);
  ResultState(const ResultState&) = delete;
  ResultState& operator=(const ResultState&) = delete;
  ResultState(ResultState&&) = default;
  ResultState& operator=(ResultState&&) = default;

  EvaluateContext m_eval_ctx;
};

class TerminalState {
 public:
  TerminalState();
  TerminalState(const TerminalState&) = delete;
  TerminalState& operator=(const TerminalState&) = delete;
  TerminalState(TerminalState&&) = default;
  TerminalState& operator=(TerminalState&&) = default;
};

using State = std::variant<TitleState, InGameState, ResultState, TerminalState>;

class StateManager {
 public:
  StateManager();
  StateManager(const StateManager&) = delete;
  StateManager& operator=(const StateManager&) = delete;
  StateManager(StateManager&&) = default;
  StateManager& operator=(StateManager&&) = default;

  bool is_running();

  void update(DrawManagerBase& draw_manager);

 private:
  State m_state;
};

#endif  // STATE_H
