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

struct PlacePipeSubState {};

struct LinkPipeSubState {
  glm::ivec2 m_point;
  int m_machine_id;
  int m_port_id;
};

struct PlaceMachineSubState {
  MachineKind m_machine;
};

struct EvaluateSubState {
  int m_time_count;
};

struct RecipeSubState {};

using SubState = std::variant<PlacePipeSubState, LinkPipeSubState, PlaceMachineSubState, EvaluateSubState, RecipeSubState>;

class State {
 public:
  State();
  virtual ~State();

  virtual State* update(DrawManagerBase& draw_manager) = 0;
};

class TitleState : public State {
 public:
  TitleState();
  ~TitleState() override;

  State* update(DrawManagerBase& draw_manager) override;
};

class InGameState : public State {
 public:
  InGameState(int stage);
  ~InGameState() override;

  State* update(DrawManagerBase& draw_manager) override;

 private:
  PipeManager m_pipe_manager;
  MachineManager m_machine_manager;
  SubState m_sub_state;
  std::mt19937 m_rng;
  EvaluateContext m_stats;
};

class ResultState : public State {
 public:
  ResultState(EvaluateContext m_game_score);
  ~ResultState() override;

  State* update(DrawManagerBase& draw_manager) override;

 private:
  EvaluateContext m_stats;
};

#endif  // _STATE_H
