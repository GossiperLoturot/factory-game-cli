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
};

class LinkPipeSubState {
 public:
  LinkPipeSubState(glm::ivec2 point, int machine_id, int port_id);

  glm::ivec2 m_point;
  int m_machine_id;
  int m_port_id;
};

class PlaceMachineSubState {
 public:
  PlaceMachineSubState(MachineKind machine);

  MachineKind m_machine;
};

class EvaluateSubState {
 public:
  EvaluateSubState();

  int m_time_count;
};

class RecipeSubState {
 public:
  RecipeSubState();
};

using SubState = std::variant<PlacePipeSubState, LinkPipeSubState, PlaceMachineSubState, EvaluateSubState, RecipeSubState>;

class TitleState {
 public:
  TitleState();
};

class InGameState {
 public:
  InGameState();

  MachineManager m_machine_manager;
  SubState m_sub_state;
  EvaluateContext m_eval_ctx;
};

class ResultState {
 public:
  ResultState(EvaluateContext&& eval_ctx);

  EvaluateContext m_eval_ctx;
};

class TerminalState {
 public:
  TerminalState();
};

using State = std::variant<TitleState, InGameState, ResultState, TerminalState>;

class StateManager {
 public:
  StateManager();

  bool is_running();

  void update(DrawManagerBase& draw_manager);

 private:
  State m_state;
};

#endif  // STATE_H
