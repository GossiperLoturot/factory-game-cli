#ifndef _STATE_H
#define _STATE_H

#include <random>

#include "draw.h"
#include "machine.h"
#include "pipe.h"

enum class MachineKind {
  ELECTROLYZER,
  CUTTER,
  LAZER,
  ASSEMBLER,
};

enum class Mode {
  PLACE_PIPE,
  LINK_PIPE,
  PLACE_MACHINE,
  EVALUATE,
  RECIPE,
};

union ModeState {
  struct {
  } PlacePipe;
  struct {
    glm::ivec2 point;
    int machine_id;
    int port_id;
  } LinkPipe;
  struct {
    MachineKind machine;
  } PlaceMachine;
  struct {
    int time_count;
  } Evaluate;
};

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
  Mode m_mode;
  ModeState m_mode_state;
  std::default_random_engine m_rng;
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
