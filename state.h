#pragma once

#include <windows.h>
#include <conio.h>

#include <string>
#include <sstream>
#include <iomanip>

#include "draw.h"
#include "pipe.h"

using namespace std;

enum Modes {
	MODE_PLACE_PIPE,
	MODE_LINK_PIPE,
	MODE_PLACE_MACHINE,
	MODE_EVALUATE,
	MODE_RECIPE,
};

union ModeState {
	struct {} place_pipe;
	struct { SHORT x; SHORT y; Anchor* anchor; } link_pipe;
	struct { Machines machine; } place_machine;
	struct { int time_count; } evaluate;
};

class State {
public:
	State();
	virtual ~State();

	virtual string get_name() = 0;
	virtual State* update(HANDLE stdin_handle, DrawManager* draw_manager) = 0;
protected:
	INPUT_RECORD* capture_input(HANDLE stdin_handle);
private:
	DWORD m_num_events;
	INPUT_RECORD m_input_record;
};

class TitleState : public State {
public:
	TitleState();
	~TitleState();

	virtual string get_name() override;
	virtual State* update(HANDLE stdin_handle, DrawManager* draw_manager) override;
};

class InGameState : public State {
public:
	InGameState(int stage);
	~InGameState();

	virtual string get_name() override;
	virtual State* update(HANDLE stdin_handle, DrawManager* draw_manager) override;
private:
	PipeManager m_pipe_manager;
	MachineManager m_machine_manager;

	Modes m_mode;
	ModeState m_mode_state;
	default_random_engine m_rng;

	ProductiveStats m_stats;
};

class ResultState : public State {
public:
	ResultState(ProductiveStats m_game_score);
	~ResultState();

	virtual string get_name() override;
	virtual State* update(HANDLE stdin_handle, DrawManager* draw_manager) override;
private:
	ProductiveStats m_stats;
};
