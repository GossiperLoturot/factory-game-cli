#include "state.h"

bool handle_input_keycode(INPUT_RECORD* input, WORD keycode)
{
	return input != nullptr && input->EventType == KEY_EVENT && input->Event.KeyEvent.bKeyDown && input->Event.KeyEvent.wVirtualKeyCode == keycode;
}

bool handle_input_mouse(INPUT_RECORD* input, WORD state, SHORT* x, SHORT* y)
{
	if (input != nullptr && input->EventType == MOUSE_EVENT && input->Event.MouseEvent.dwEventFlags == 0 && input->Event.MouseEvent.dwButtonState & state)
	{
		if (x != nullptr) *x = input->Event.MouseEvent.dwMousePosition.X;
		if (y != nullptr) *y = input->Event.MouseEvent.dwMousePosition.Y;
		return true;
	}

	return false;
}

// BASE STATE

State::State() : m_input_record({}), m_num_events(0) {
}

State::~State() {}

INPUT_RECORD* State::capture_input(HANDLE stdin_handle) {
	GetNumberOfConsoleInputEvents(stdin_handle, &m_num_events);

	if (m_num_events > 0) {
		ReadConsoleInput(stdin_handle, &m_input_record, 1, &m_num_events);

		return &m_input_record;
	}

	return nullptr;
}

// TITLE STATE

TitleState::TitleState() : State::State() {}

TitleState::~TitleState() {}

string TitleState::get_name() { return "Title"; }

State* TitleState::update(HANDLE stdin_handle, DrawManager* draw_manager) {
	draw_manager->Clear();

	draw_manager->DrawLabel(10, 5, "Anata ha seisan rainn no sekkei wo ukewou kaisya no syainn desu.");
	draw_manager->DrawLabel(10, 6, "Tugi tugi to maikomu irai ni taisite seigen zikan nai ni saiteki na seisan rainn wo sekkei site kudasai.");
	draw_manager->DrawLabel(10, 7, "koku ikkoku to semaru zikan no naka de saiteki kai wo syunji ni mitibiki dasou!");

	draw_manager->DrawLineBox(0, 0, draw_manager->m_width, draw_manager->m_height - 2);
	draw_manager->DrawLabelBox(1, 1, get_name());
	draw_manager->Present();

	INPUT_RECORD* input = capture_input(stdin_handle);
	if (handle_input_keycode(input, VK_RETURN)) {
		return new InGameState(1);
	}
	if (handle_input_keycode(input, VK_ESCAPE)) {
		return nullptr;
	}
	if (handle_input_mouse(input, FROM_LEFT_1ST_BUTTON_PRESSED, nullptr, nullptr)) {
		return new InGameState(1);
	}

	return this;
}

// IN-GAME STATE

InGameState::InGameState(int stage) : State::State(), m_pipe_manager(), m_machine_manager(), m_mode(MODE_PLACE_PIPE), m_mode_state({}), m_rng(random_device()()), m_stats()
{
	m_stats.stage = stage;
	m_stats.design_time = 60 * 60;

	// Stage 1.
	if (stage == 1)
	{
		{
			auto machine = make_shared<InputM>(InputM(Point(50, 5), ITEM_WATER));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
		{
			auto machine = make_shared<OutputM>(OutputM(Point(30, 25), ITEM_HYDROGEN));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
		{
			auto machine = make_shared<OutputM>(OutputM(Point(70, 25), ITEM_OXYGEN));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
	}

	// Stage 2.
	if (stage == 2)
	{
		{
			auto machine = make_shared<InputM>(InputM(Point(30, 5), ITEM_SILICON));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
		{
			auto machine = make_shared<InputM>(InputM(Point(50, 5), ITEM_SOLDERING_IRON));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
		{
			auto machine = make_shared<InputM>(InputM(Point(70, 5), ITEM_CIRCUIT_BOARD));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
		{
			auto machine = make_shared<OutputM>(OutputM(Point(50, 25), ITEM_CHIP));
			machine->upgrade_anchors();
			m_machine_manager.AddMachine(machine);
		}
	}
}

InGameState::~InGameState() {}

string InGameState::get_name() {
	return "In Game";
}

State* InGameState::update(HANDLE stdin_handle, DrawManager* draw_manager) {
	draw_manager->Clear();

	m_pipe_manager.Draw(draw_manager);
	m_machine_manager.Draw(draw_manager);

	INPUT_RECORD* input = capture_input(stdin_handle);
	if (handle_input_keycode(input, VK_TAB)) {
		if (m_mode == MODE_PLACE_PIPE || m_mode == MODE_LINK_PIPE) {
			m_mode = MODE_PLACE_MACHINE;
			m_mode_state.place_machine = { MACHINE_ELECTROLYZER };
		}
		else if (m_mode == MODE_PLACE_MACHINE) {
			m_mode = MODE_PLACE_PIPE;
			m_mode_state.place_pipe = {};
		}
	}
	if (handle_input_keycode(input, VK_RETURN)) {
		if (m_mode != MODE_EVALUATE) {
			m_mode = MODE_EVALUATE;
			m_mode_state.evaluate = { 0 };
		}
	}
	if (handle_input_keycode(input, 'R')) {
		if (m_mode != MODE_EVALUATE)
		{
			if (m_mode == MODE_RECIPE) m_mode = MODE_PLACE_PIPE;
			else if (m_mode != MODE_RECIPE) m_mode = MODE_RECIPE;
		}
	}

	SHORT x, y;
	switch (m_mode) {
	case MODE_PLACE_PIPE:
	{
		draw_manager->DrawLabel(1, draw_manager->m_height - 2, "Place Pipe");
		draw_manager->DrawLabel(1, draw_manager->m_height - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

		if (handle_input_mouse(input, FROM_LEFT_1ST_BUTTON_PRESSED, &x, &y)) {
			Anchor* anchor = m_machine_manager.GetAnchor(Point(x, y));

			if (anchor != nullptr)
			{
				m_mode = MODE_LINK_PIPE;
				m_mode_state.link_pipe = { x, y, anchor };
			}
		}
		break;
	}
	case MODE_LINK_PIPE:
	{
		draw_manager->DrawLabel(1, draw_manager->m_height - 2, "Link Pipe");
		draw_manager->DrawLabel(1, draw_manager->m_height - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe");

		draw_manager->DrawLabel(m_mode_state.link_pipe.x, m_mode_state.link_pipe.y, "X");

		if (handle_input_mouse(input, FROM_LEFT_1ST_BUTTON_PRESSED, &x, &y)) {
			Point point = Point(x, y);

			Anchor* anchor = m_machine_manager.GetAnchor(point);
			if (anchor != nullptr)
			{
				// 生産ラインのの入出力の関連付け
				Anchor* begin_anchor = m_mode_state.link_pipe.anchor;
				Anchor* end_anchor = anchor;
				shared_ptr<Pipe> pipe = make_shared<Pipe>(Pipe(Point(m_mode_state.link_pipe.x, m_mode_state.link_pipe.y), point, begin_anchor, end_anchor));
				m_pipe_manager.AddPipe(pipe);
				begin_anchor->m_piped_anchors.push_back(end_anchor);
				begin_anchor->m_pipes.push_back(pipe);
				end_anchor->m_piped_anchors.push_back(begin_anchor);
				end_anchor->m_pipes.push_back(pipe);

				m_mode = MODE_PLACE_PIPE;
				m_mode_state.place_pipe = {};
			}
		}
		break;
	}
	case MODE_PLACE_MACHINE:
	{
		draw_manager->DrawLabel(1, draw_manager->m_height - 2, "Place Machine");
		draw_manager->DrawLabel(1, draw_manager->m_height - 1, "LClick: Place, RClick: Remove, Tab: Change Mode, Enter: Submit, Esc: Quit, R: Recipe, Space: Change Machine");
		if (m_mode_state.place_machine.machine == MACHINE_ELECTROLYZER) {
			draw_manager->DrawLabel(15, draw_manager->m_height - 2, "[Electrolyzer]");
		}
		else if (m_mode_state.place_machine.machine == MACHINE_CUTTER) {
			draw_manager->DrawLabel(15, draw_manager->m_height - 2, "[Cutter]");
		}
		else if (m_mode_state.place_machine.machine == MACHINE_LAZER) {
			draw_manager->DrawLabel(15, draw_manager->m_height - 2, "[Lazer]");
		}
		else if (m_mode_state.place_machine.machine == MACHINE_ASSEMBLER) {
			draw_manager->DrawLabel(15, draw_manager->m_height - 2, "[Assembler]");
		}

		if (handle_input_keycode(input, VK_SPACE)) {
			if (m_mode_state.place_machine.machine == MACHINE_ELECTROLYZER) {
				m_mode_state.place_machine.machine = MACHINE_CUTTER;
			}
			else if (m_mode_state.place_machine.machine == MACHINE_CUTTER) {
				m_mode_state.place_machine.machine = MACHINE_LAZER;
			}
			else if (m_mode_state.place_machine.machine == MACHINE_LAZER) {
				m_mode_state.place_machine.machine = MACHINE_ASSEMBLER;
			}
			else if (m_mode_state.place_machine.machine == MACHINE_ASSEMBLER) {
				m_mode_state.place_machine.machine = MACHINE_ELECTROLYZER;
			}
		}

		if (handle_input_mouse(input, FROM_LEFT_1ST_BUTTON_PRESSED, &x, &y)) {
			Point point = Point(x, y);

			if (m_mode_state.place_machine.machine == MACHINE_ELECTROLYZER)
			{
				shared_ptr<Machine> machine = make_shared<ElectrolyzerM>(ElectrolyzerM(point));
				machine->upgrade_anchors();
				m_machine_manager.AddMachine(machine);
			}
			else if (m_mode_state.place_machine.machine == MACHINE_CUTTER)
			{
				shared_ptr<Machine> machine = make_shared<CutterM>(CutterM(point));
				machine->upgrade_anchors();
				m_machine_manager.AddMachine(machine);
			}
			else if (m_mode_state.place_machine.machine == MACHINE_LAZER)
			{
				shared_ptr<Machine> machine = make_shared<LaserM>(LaserM(point));
				machine->upgrade_anchors();
				m_machine_manager.AddMachine(machine);
			}
			else if (m_mode_state.place_machine.machine == MACHINE_ASSEMBLER)
			{
				shared_ptr<Machine> machine = make_shared<AssemblerM>(AssemblerM(point));
				machine->upgrade_anchors();
				m_machine_manager.AddMachine(machine);
			}
		}
		break;
	}
	}
	if (m_mode != MODE_EVALUATE && m_mode != MODE_RECIPE)
	{
		if (handle_input_mouse(input, RIGHTMOST_BUTTON_PRESSED, &x, &y)) {
			if (m_mode == MODE_LINK_PIPE) {
				m_mode = MODE_PLACE_PIPE;
				m_mode_state.place_pipe = {};
			}

			shared_ptr<Pipe> pipe;
			shared_ptr<Machine> machine;

			pipe = m_pipe_manager.RemovePipe(Point(x, y));
			if (pipe != 0)
			{
				Anchor* begin = pipe->begin_anchor;
				Anchor* end = pipe->end_anchor;
				{
					vector<shared_ptr<Pipe>> pipes = begin->m_pipes;
					pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
					vector<Anchor*> anchors = begin->m_piped_anchors;
					anchors.erase(remove(anchors.begin(), anchors.end(), end), anchors.end());
				}
				{
					vector<shared_ptr<Pipe>> pipes = end->m_pipes;
					pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
					vector<Anchor*> anchors = end->m_piped_anchors;
					anchors.erase(remove(anchors.begin(), anchors.end(), begin), anchors.end());
				}

				goto OUTSIDE;
			}
			machine = m_machine_manager.RemoveMachine(Point(x, y));
			if (machine != 0)
			{
				vector<Anchor*> machine_anchors = machine->get_anchors();
				for (size_t i = 0; i < machine_anchors.size(); ++i)
				{
					Anchor* machine_anchor = machine_anchors[i];
					for (size_t j = 0; j < machine_anchor->m_pipes.size(); ++j)
					{
						shared_ptr<Pipe> child_pipe = machine_anchor->m_pipes[j];
						m_pipe_manager.RemovePipe(child_pipe);

						if (child_pipe != 0)
						{
							Anchor* begin = child_pipe->begin_anchor;
							Anchor* end = child_pipe->end_anchor;
							{
								vector<shared_ptr<Pipe>> pipes = begin->m_pipes;
								pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
								vector<Anchor*> anchors = begin->m_piped_anchors;
								anchors.erase(remove(anchors.begin(), anchors.end(), end), anchors.end());
							}
							{
								vector<shared_ptr<Pipe>> pipes = end->m_pipes;
								pipes.erase(remove(pipes.begin(), pipes.end(), pipe), pipes.end());
								vector<Anchor*> anchors = end->m_piped_anchors;
								anchors.erase(remove(anchors.begin(), anchors.end(), begin), anchors.end());
							}
						}
					}
				}

				goto OUTSIDE;
			}
		OUTSIDE:;
		}

		--m_stats.design_time;
	}
	if (m_mode == MODE_EVALUATE)
	{
		ostringstream status_stream;
		status_stream << "Evaluating... : " << setprecision(2) << fixed << (static_cast<float>(m_mode_state.evaluate.time_count) / 60.0f) << " / 3.00";
		string status = status_stream.str();

		draw_manager->DrawLabelBox(50, 1, status);

		m_mode_state.evaluate.time_count++;
		if (m_mode_state.evaluate.time_count <= 60 * 3) {
			m_machine_manager.Evaluate(&m_stats, m_rng);
		}
		else
		{
			return new ResultState(m_stats);
		}
	}
	if (m_mode == MODE_RECIPE)
	{
		draw_manager->DrawClearBox(20, 4, 80, 20);
		draw_manager->DrawLineBox(20, 4, 80, 20);
		draw_manager->DrawLabelBox(21, 5, "Recipe Book : R to Exit");

		draw_manager->DrawLabel(22, 8, "[Electrolyzer]");
		draw_manager->DrawLabel(22, 9, "Input : Water");
		draw_manager->DrawLabel(22, 10, "Output 1 : Hydrogen");
		draw_manager->DrawLabel(22, 11, "Output 2 : Oxygen");

		draw_manager->DrawLabel(22, 13, "[Cutter]");
		draw_manager->DrawLabel(22, 14, "Input : Silicon");
		draw_manager->DrawLabel(22, 15, "Output : Silicon Wafer");

		draw_manager->DrawLabel(22, 17, "[Cutter]");
		draw_manager->DrawLabel(22, 18, "Input : Circuit Wafer");
		draw_manager->DrawLabel(22, 19, "Output : Circuit");

		draw_manager->DrawLabel(52, 8, "[Laser]");
		draw_manager->DrawLabel(52, 9, "Input : Silicon Wafer");
		draw_manager->DrawLabel(52, 10, "Output : Circuit Wafer");

		draw_manager->DrawLabel(52, 12, "[Assembler]");
		draw_manager->DrawLabel(52, 13, "Input 1 : Circuit");
		draw_manager->DrawLabel(52, 14, "Input 2 : Soldering Iron");
		draw_manager->DrawLabel(52, 15, "Input 3 : Circuit Board");
		draw_manager->DrawLabel(52, 16, "Output : Chip");
	}

	ostringstream time_stream;
	time_stream << "Time : " << (m_stats.design_time / 60) << ":" << setw(2) << setfill('0') << (m_stats.design_time % 60);
	string time = time_stream.str();
	draw_manager->DrawLabelBox(draw_manager->m_width - 1 - static_cast<int>(time.size()), draw_manager->m_height - 4, time);

	draw_manager->DrawLineBox(0, 0, draw_manager->m_width, draw_manager->m_height - 2);
	draw_manager->DrawLabelBox(1, 1, get_name());
	draw_manager->Present();

	if (handle_input_keycode(input, VK_ESCAPE)) {
		return new ResultState(m_stats);
	}

	return this;
}

// RESULT STATE

ResultState::ResultState(ProductiveStats stats) : State::State(), m_stats(stats) {}

ResultState::~ResultState() {}

string ResultState::get_name() {
	return "Result";
}

// ゲームの結果標示、処理は雑
State* ResultState::update(HANDLE stdin_handle, DrawManager* draw_manager) {
	draw_manager->Clear();

	draw_manager->DrawLabelBox(30, 10, "Game Result");

	ostringstream time_stream;
	time_stream << "Time : " << (m_stats.design_time / 60) << ":" << setw(2) << setfill('0') << (m_stats.design_time % 60) << " / 60:00";
	string time = time_stream.str();
	draw_manager->DrawLabel(30, 14, time);

	bool is_perfect = true;
	bool is_bad_inv = false;
	float score_value = 0.0f;
	for (size_t i = 0; i < m_stats.items.size(); ++i) {
		ostringstream line_stream;
		line_stream << item_to_string(m_stats.items[i]) << " : " << m_stats.counts[i] << " unit.";
		string line = line_stream.str();
		draw_manager->DrawLabel(30, 16 + static_cast<SHORT>(i), line);

		is_perfect &= (m_stats.counts[i] > 0);
		is_bad_inv |= (m_stats.counts[i] > 0);

		score_value += static_cast<float>(m_stats.counts[i]);
	}
	score_value *= (static_cast<float>(m_stats.design_time) / 3600.0f);
	ostringstream score_stream;
	score_stream << "Score : " << setprecision(2) << fixed << score_value;
	string score = score_stream.str();
	draw_manager->DrawLabel(30, 12, score);

	if (!is_bad_inv) draw_manager->DrawLabel(43, 10, "Bad...");
	else if (!is_perfect) draw_manager->DrawLabel(43, 10, "Good!");
	else draw_manager->DrawLabel(43, 10, "Perfect!!!");

	draw_manager->DrawLineBox(0, 0, draw_manager->m_width, draw_manager->m_height - 2);
	draw_manager->DrawLabelBox(1, 1, get_name());
	draw_manager->Present();

	INPUT_RECORD* input = capture_input(stdin_handle);
	if (handle_input_keycode(input, VK_RETURN)) {
		if (m_stats.stage == 1 && is_bad_inv) return new InGameState(2);
		else return nullptr;
	}
	if (handle_input_keycode(input, VK_ESCAPE)) {
		return nullptr;
	}
	if (handle_input_mouse(input, FROM_LEFT_1ST_BUTTON_PRESSED, nullptr, nullptr)) {
		if (m_stats.stage == 1 && is_bad_inv) return new InGameState(2);
		else return nullptr;
	}

	return this;
}