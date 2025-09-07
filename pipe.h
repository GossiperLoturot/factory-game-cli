#pragma once

#include <vector>
#include <memory>

#include "draw.h"
#include "foundation.h"
#include "machine.h"

using namespace std;

class Anchor; // for pointer reference
class Pipe {
public:
	Point begin;
	Point end;
	Anchor* begin_anchor;
	Anchor* end_anchor;

	Pipe(Point begin, Point end, Anchor* begin_anchor, Anchor* end_anchor);
	~Pipe();

	string get_name();
	void Draw(DrawManager* draw_manager);
	void CreateIdx(size_t* buffer, size_t self_idx);
};

class PipeManager
{
public:
	PipeManager();
	~PipeManager();

	void Draw(DrawManager* draw_manager);
	void AddPipe(shared_ptr<Pipe> pipe);
	void RemovePipe(shared_ptr<Pipe> point);
	shared_ptr<Pipe> RemovePipe(Point point);
private:
	vector<shared_ptr<Pipe>> m_pipes;
	vector<size_t> m_field;
};
