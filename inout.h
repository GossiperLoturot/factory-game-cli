#pragma once

#include <vector>
#include <memory>

#include "draw.h"
#include "foundation.h"
#include "pipe.h"

using namespace std;

class Input {
public:
	Point point;
	Item m_item;

	Input(Point point, Item item);
	~Input();

	virtual string get_name() = 0;
	virtual void Draw(DrawManager* draw_manager) = 0;
	virtual void CreateAnchor(Anchor** buffer) = 0;
};

class Output {
public:
	Point point;
	Item m_item;

	Output(Point point, Item item);
	~Output();

	virtual string get_name() = 0;
	virtual void Draw(DrawManager* draw_manager) = 0;
	virtual void CreateAnchor(Anchor** buffer) = 0;
};

class InoutManager
{
public:
	InoutManager();
	~InoutManager();

	void Draw(DrawManager* draw_manager);
	void AddInput(shared_ptr<Input> input);
	void AddOutput(shared_ptr<Output> output);
	Anchor* GetAnchor(Point point);
private:
	vector<shared_ptr<Input>> m_inputs;
	vector<shared_ptr<Output>> m_outputs;
	vector<Anchor*> m_anchor_idx;
};
