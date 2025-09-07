#include "state.h"
#include "draw.h"
#include "pool.h"

#include <thread>
#include <atomic>
#include <sstream>

atomic_bool is_running(false);
atomic_bool ok_winsize(false);
const SHORT MIN_WIDTH = 120;
const SHORT MIN_HEIGHT = 30;

void check_winsize(HANDLE stdout_handle)
{
	CONSOLE_SCREEN_BUFFER_INFO screen;

	while (is_running) {
		if (GetConsoleScreenBufferInfo(stdout_handle, &screen)) {
			int width = screen.srWindow.Right - screen.srWindow.Left + 1;
			int height = screen.srWindow.Bottom - screen.srWindow.Top + 1;
			ok_winsize = (width >= MIN_WIDTH && height >= MIN_HEIGHT);
		}

		Sleep(100);
	}
}

int main()
{
	HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD out_mode = 0;
	GetConsoleMode(stdout_handle, &out_mode);
	SetConsoleMode(stdout_handle, out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD in_mode = 0;
	GetConsoleMode(stdin_handle, &in_mode);
	SetConsoleMode(stdin_handle, (in_mode | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS) & ~ENABLE_QUICK_EDIT_MODE);

	DrawManager draw_manager = DrawManager(MIN_WIDTH, MIN_HEIGHT, stdout_handle);

	is_running = true;
	std::thread check_winsize_thread(check_winsize, stdout_handle);

	State* state = new TitleState();

	do {
		if (ok_winsize) {
			State* new_state = state->update(stdin_handle, &draw_manager);

			if (new_state != state) {
				delete state;
				state = new_state;
			}
		}
		else
		{
			ostringstream msg_stream;
			msg_stream << "Please resize the window least at " << MIN_WIDTH << "x" << MIN_HEIGHT << ".";
			string msg = msg_stream.str();

			draw_manager.Clear();
			draw_manager.DrawLabel(0, 0, msg);
			draw_manager.Present();
		}

		Sleep(16);
	} while (state != nullptr);

	is_running = false;
	check_winsize_thread.join();

	return EXIT_SUCCESS;
}