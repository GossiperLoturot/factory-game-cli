#pragma once

#include <vector>
#include <memory>
#include <random>
#include <algorithm>

#include "draw.h"
#include "foundation.h"
#include "pipe.h"
#include "pool.h"

using namespace std;

struct ProductiveStats {
	int stage;
	int design_time;
	vector<Item> items;
	vector<int> counts;
};

enum Machines {
	MACHINE_ELECTROLYZER,
	MACHINE_CUTTER,
	MACHINE_LAZER,
	MACHINE_ASSEMBLER,
};

class Machine; // for pointer reference
class Pipe;    // for pointer reference
class Anchor {
public:
	Machine* m_parent;
	vector<shared_ptr<Pipe>> m_pipes;
	vector<Anchor*> m_piped_anchors;

	Anchor();
	Anchor(Machine* parent);
	~Anchor();
};

class Machine {
public:
	Point point;

	Machine(Point point);
	virtual ~Machine();

	// �q�[�v�̈�Ɉړ������ۂɃ|�C���^���Đݒ�A�ʂ̃A�v���[�`���l������
	virtual void upgrade_anchors() = 0;

	virtual bool is_breakable() = 0;
	virtual string get_name() = 0;
	virtual vector<Anchor*> get_anchors() = 0;

	virtual void Draw(DrawManager* draw_manager) = 0;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) = 0;
	virtual void CreateAnchor(Anchor** buffer) = 0;

	virtual void InsertItem(Item given_item) = 0;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) = 0;
};

class InputM : public Machine {
public:
	InputM(Point point, Item item);
	~InputM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	PoolAllocator<Anchor, 100> pool;

	Item item;
	Anchor m_output;
};

class OutputM : public Machine {
public:
	OutputM(Point point, Item item);
	~OutputM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	Item item;
	Anchor m_input;

	int m_stored_count;
};

class ElectrolyzerM : public Machine {
public:
	ElectrolyzerM(Point point);
	~ElectrolyzerM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	Anchor m_input;
	Anchor m_output0;
	Anchor m_output1;

	int m_stored_count;
};

class CutterM : public Machine {
public:
	CutterM(Point point);
	~CutterM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	Anchor m_input;
	Anchor m_output;

	int m_stored_count[2];
};

class LaserM : public Machine {
public:
	LaserM(Point point);
	~LaserM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	Anchor m_input;
	Anchor m_output;

	int m_stored_count;
};

class AssemblerM : public Machine {
public:
	AssemblerM(Point point);
	~AssemblerM();

	virtual void upgrade_anchors() override;

	virtual bool is_breakable() override;
	virtual string get_name() override;
	virtual vector<Anchor*> get_anchors() override;

	virtual void Draw(DrawManager* draw_manager) override;
	virtual void CreateIdx(size_t* buffer, size_t self_idx) override;
	virtual void CreateAnchor(Anchor** buffer) override;

	virtual void InsertItem(Item given_item) override;
	virtual void Evaluate(ProductiveStats* stats, default_random_engine rng) override;
private:
	Anchor m_input0;
	Anchor m_input1;
	Anchor m_input2;
	Anchor m_output;

	int m_stored_count[3];
};

class MachineManager
{
public:
	MachineManager();
	~MachineManager();

	void Draw(DrawManager* draw_manager);
	void AddMachine(shared_ptr<Machine> machine);
	shared_ptr<Machine> RemoveMachine(Point point);

	Anchor* GetAnchor(Point point);

	void Evaluate(ProductiveStats* stats, default_random_engine rng);
private:
	vector<shared_ptr<Machine>> m_machines;
	vector<size_t> m_sparse_idx;
	vector<Anchor*> m_anchor_idx;
};
