#ifndef _STATE_H
#define _STATE_H

#include <random>
#include <variant>

#include "draw.h"
#include "machine.h"
#include "pipe.h"

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

  PipeManager m_pipe_manager;
  MachineManager m_machine_manager;
  SubState m_sub_state;
  std::mt19937 m_rng;
  EvaluateContext m_stats;
};

class ResultState {
 public:
  ResultState(EvaluateContext stats);

  EvaluateContext m_stats;
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

#endif  // _STATE_H
