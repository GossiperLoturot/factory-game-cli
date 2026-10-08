#ifndef STATE_H
#define STATE_H

#include <variant>

#include "draw.h"
#include "machine.h"

struct PlacePipeSubState {
  PlacePipeSubState();
  PlacePipeSubState(const PlacePipeSubState&) = delete;
  PlacePipeSubState& operator=(const PlacePipeSubState&) = delete;
  PlacePipeSubState(PlacePipeSubState&&) = default;
  PlacePipeSubState& operator=(PlacePipeSubState&&) = default;
};

struct LinkPipeSubState {
  glm::ivec2 m_point;
  int m_machine_id;
  int m_port_id;

  LinkPipeSubState(glm::ivec2 point, int machine_id, int port_id);
  LinkPipeSubState(const LinkPipeSubState&) = delete;
  LinkPipeSubState& operator=(const LinkPipeSubState&) = delete;
  LinkPipeSubState(LinkPipeSubState&&) = default;
  LinkPipeSubState& operator=(LinkPipeSubState&&) = default;
};

struct PlaceMachineSubState {
  MachineKind m_machine;

  PlaceMachineSubState(MachineKind machine);
  PlaceMachineSubState(const PlaceMachineSubState&) = delete;
  PlaceMachineSubState& operator=(const PlaceMachineSubState&) = delete;
  PlaceMachineSubState(PlaceMachineSubState&&) = default;
  PlaceMachineSubState& operator=(PlaceMachineSubState&&) = default;
};

struct EvaluateSubState {
  int m_time_count;

  EvaluateSubState();
  EvaluateSubState(const EvaluateSubState&) = delete;
  EvaluateSubState& operator=(const EvaluateSubState&) = delete;
  EvaluateSubState(EvaluateSubState&&) = default;
  EvaluateSubState& operator=(EvaluateSubState&&) = default;
};

struct RecipeSubState {
  RecipeSubState();
  RecipeSubState(const RecipeSubState&) = delete;
  RecipeSubState& operator=(const RecipeSubState&) = delete;
  RecipeSubState(RecipeSubState&&) = default;
  RecipeSubState& operator=(RecipeSubState&&) = default;
};

struct SubmitSubState {
  SubmitSubState();
  SubmitSubState(const SubmitSubState&) = delete;
  SubmitSubState& operator=(const SubmitSubState&) = delete;
  SubmitSubState(SubmitSubState&&) = default;
  SubmitSubState& operator=(SubmitSubState&&) = default;
};

using SubState = std::variant<PlacePipeSubState, LinkPipeSubState, PlaceMachineSubState, EvaluateSubState, RecipeSubState, SubmitSubState>;

SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, PlacePipeSubState&& substate);
SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, LinkPipeSubState&& substate);
SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, PlaceMachineSubState&& substate);
SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, EvaluateSubState&& substate);
SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, RecipeSubState&& substate);
SubState update(DrawManagerBase& draw_manager, MachineManager& machine_manager, EvaluateContext& eval_ctx, SubmitSubState&& substate);
bool is_terminate(const PlacePipeSubState& substate);
bool is_terminate(const LinkPipeSubState& substate);
bool is_terminate(const PlaceMachineSubState& substate);
bool is_terminate(const EvaluateSubState& substate);
bool is_terminate(const RecipeSubState& substate);
bool is_terminate(const SubmitSubState& substate);

struct TitleState {
  TitleState();
  TitleState(const TitleState&) = delete;
  TitleState& operator=(const TitleState&) = delete;
  TitleState(TitleState&&) = default;
  TitleState& operator=(TitleState&&) = default;
};

struct InGameState {
  MachineManager m_machine_manager;
  SubState m_substate;
  EvaluateContext m_eval_ctx;

  InGameState();
  InGameState(const InGameState&) = delete;
  InGameState& operator=(const InGameState&) = delete;
  InGameState(InGameState&&) = default;
  InGameState& operator=(InGameState&&) = default;

  static InGameState new_lv1();
  static InGameState new_lv2();
};

struct ResultState {
  EvaluateContext m_eval_ctx;

  ResultState(EvaluateContext eval_ctx);
  ResultState(const ResultState&) = delete;
  ResultState& operator=(const ResultState&) = delete;
  ResultState(ResultState&&) = default;
  ResultState& operator=(ResultState&&) = default;
};

struct TerminalState {
  TerminalState();
  TerminalState(const TerminalState&) = delete;
  TerminalState& operator=(const TerminalState&) = delete;
  TerminalState(TerminalState&&) = default;
  TerminalState& operator=(TerminalState&&) = default;
};

using State = std::variant<TitleState, InGameState, ResultState, TerminalState>;

State update(DrawManagerBase& draw_manager, TitleState&& state);
State update(DrawManagerBase& draw_manager, InGameState&& state);
State update(DrawManagerBase& draw_manager, ResultState&& state);
State update(DrawManagerBase& draw_manager, TerminalState&& state);
bool is_terminate(const TitleState& state);
bool is_terminate(const InGameState& state);
bool is_terminate(const ResultState& state);
bool is_terminate(const TerminalState& state);

class StateManager {
 public:
  StateManager();
  StateManager(const StateManager&) = delete;
  StateManager& operator=(const StateManager&) = delete;
  StateManager(StateManager&&) = default;
  StateManager& operator=(StateManager&&) = default;

  bool is_terminate();
  void update(DrawManagerBase& draw_manager);

 private:
  State m_state;
};

#endif  // STATE_H
