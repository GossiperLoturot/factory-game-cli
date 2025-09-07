#pragma once

#include <string>

#include "draw.h"
#include "machine.h"
#include "pipe.h"

namespace factory_game {

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
    Anchor* anchor;
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
  State() = default;
  virtual ~State() = default;

  virtual std::string get_name() = 0;
  virtual State* update(DrawManagerBase* draw_manager) = 0;
};

class TitleState : public State {
 public:
  TitleState();
  ~TitleState() override;

  std::string get_name() override;
  State* update(DrawManagerBase* draw_manager) override;
};

class InGameState : public State {
 public:
  InGameState(int stage);
  ~InGameState() override;

  std::string get_name() override;
  State* update(DrawManagerBase* draw_manager) override;

 private:
  PipeManager m_pipe_manager;
  MachineManager m_machine_manager;
  Modes m_mode;
  ModeState m_mode_state;
  std::default_random_engine m_rng;
  ProductiveStats m_stats;
};

class ResultState : public State {
 public:
  ResultState(ProductiveStats m_game_score);
  ~ResultState() override;

  std::string get_name() override;
  State* update(DrawManagerBase* draw_manager) override;

 private:
  ProductiveStats m_stats;
};

}  // namespace factory_game
