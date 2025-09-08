#pragma once

#include <random>
#include <string>

#include "draw.h"
#include "machine.h"
#include "pipe.h"

namespace factory_game {

enum Machines {
  MACHINE_ELECTROLYZER,
  MACHINE_CUTTER,
  MACHINE_LAZER,
  MACHINE_ASSEMBLER,
};

enum Modes {
  MODE_PLACE_PIPE,
  MODE_LINK_PIPE,
  MODE_PLACE_MACHINE,
  MODE_EVALUATE,
  MODE_RECIPE,
};

union ModeState {
  struct {
  } PlacePipe;
  struct {
    int x;
    int y;
  } LinkPipe;
  struct {
    Machines machine;
  } PlaceMachine;
  struct {
    int time_count;
  } Evaluate;
};

class State {
 public:
  State();
  virtual ~State();

  virtual State* update(DrawManagerBase* draw_manager) = 0;
};

class TitleState : public State {
 public:
  TitleState();
  ~TitleState() override;

  State* update(DrawManagerBase* draw_manager) override;
};

class InGameState : public State {
 public:
  InGameState(int stage);
  ~InGameState() override;

  State* update(DrawManagerBase* draw_manager) override;

 private:
  PipeManager m_pipe_manager;
  MachineManager m_machine_manager;
  Modes m_mode;
  ModeState m_mode_state;
  std::default_random_engine m_rng;
  EvaluateContext m_stats;
};

class ResultState : public State {
 public:
  ResultState(EvaluateContext m_game_score);
  ~ResultState() override;

  State* update(DrawManagerBase* draw_manager) override;

 private:
  EvaluateContext m_stats;
};

}  // namespace factory_game
