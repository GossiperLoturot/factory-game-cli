#pragma once

#include <windows.h>

#include <vector>
#include <string>

using namespace std;

class DrawManager {
public:
	SHORT m_width;
	SHORT m_height;
	HANDLE m_stdout_handle;

	DrawManager(SHORT m_width, SHORT m_height, HANDLE m_stdout_handle);
	~DrawManager();

	void Clear();
	void DrawLabel(int x, int y, string text);
	void DrawLabelBox(int x, int y, string text);
	void DrawClearBox(int x, int y, int width, int height);
	void DrawLineBox(int x, int y, int width, int height);
	void DrawHVLine(int x0, int y0, int x1, int y1);
	void Present();
private:
	vector<char> m_current_buffer;
	vector<char> m_back_buffer;
};
