#include "machine.h"

Anchor::Anchor() : m_parent(nullptr), m_pipes(), m_piped_anchors() {}

Anchor::Anchor(Machine* parent) : m_parent(parent), m_pipes(), m_piped_anchors() {}

Anchor::~Anchor() {}

// BASE MACHINE

Machine::Machine(Point point) : point(point) {}

Machine::~Machine() {}

// INPUT MACHINE

InputM::InputM(Point point, Item item) : Machine(point), item(item), m_output(this), pool() {
}

InputM::~InputM() {}

void InputM::upgrade_anchors() {
	m_output.m_parent = this;
}

bool InputM::is_breakable() {
	return false;
}

string InputM::get_name() {
	return "Input";
}

vector<Anchor*> InputM::get_anchors() {
	return { &m_output };
}

void InputM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x + 2, point.y - 1, item_to_string(item));
	draw_manager->DrawLabel(point.x, point.y, "[[Input]]");
	draw_manager->DrawLabel(point.x + 5, point.y + 1, "O");
}

void InputM::CreateIdx(size_t* buffer, size_t self_idx) {
	buffer, self_idx;
}

void InputM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y + 1) * 120 + point.x + 5] = &m_output;
}

void InputM::InsertItem(Item given_item) {
	given_item;
}

void InputM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	stats;

	auto anchors = m_output.m_piped_anchors;
	if (anchors.size() > 0) {
		uniform_int_distribution<> dist(0, static_cast<int>(anchors.size() - 1));
		Anchor* target = anchors[dist(rng)];
		target->m_parent->InsertItem(item);
	}
}

// OUTPUT MACHINE

OutputM::OutputM(Point point, Item item) : Machine(point), item(item), m_input(this), m_stored_count(0) {}

OutputM::~OutputM() {}

void OutputM::upgrade_anchors() {
	m_input.m_parent = this;
}

bool OutputM::is_breakable() {
	return false;
}

string OutputM::get_name() {
	return "Output";
}

vector<Anchor*> OutputM::get_anchors() {
	return { &m_input };
}

void OutputM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x + 2, point.y + 1, item_to_string(item));
	draw_manager->DrawLabel(point.x, point.y, "[[Output]]");
	draw_manager->DrawLabel(point.x + 5, point.y - 1, "I");
}

void OutputM::CreateIdx(size_t* buffer, size_t self_idx) {
	buffer, self_idx;
}

void OutputM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y - 1) * 120 + point.x + 5] = &m_input;
}

void OutputM::InsertItem(Item given_item) {
	if (given_item == item) {
		m_stored_count++;
	}
}

void OutputM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	rng;

	auto it = find(stats->items.begin(), stats->items.end(), item);
	if (it != stats->items.end()) {
		size_t index = distance(stats->items.begin(), it);
		stats->counts[index] += m_stored_count;
	}
	else
	{
		stats->items.push_back(item);
		stats->counts.push_back(m_stored_count);
	}
}

// ELECTROLYZER MACHINE

ElectrolyzerM::ElectrolyzerM(Point point) : Machine(point), m_input(this), m_output0(this), m_output1(this), m_stored_count(0) {}

ElectrolyzerM::~ElectrolyzerM() {}

void ElectrolyzerM::upgrade_anchors() {
	m_input.m_parent = this;
	m_output0.m_parent = this;
	m_output1.m_parent = this;
}

bool ElectrolyzerM::is_breakable() {
	return true;
}

string ElectrolyzerM::get_name() {
	return "Electrolyzer";
}

vector<Anchor*> ElectrolyzerM::get_anchors() {
	return { &m_input, &m_output0, &m_output1 };
}

void ElectrolyzerM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x, point.y, "[[Electrolyzer]]");
	draw_manager->DrawLabel(point.x + 7, point.y - 1, "I");
	draw_manager->DrawLabel(point.x + 5, point.y + 1, "O1");
	draw_manager->DrawLabel(point.x + 10, point.y + 1, "O2");
}

void ElectrolyzerM::CreateIdx(size_t* buffer, size_t self_idx) {
	for (int y = point.y; y < point.y + 1; ++y) {
		for (int x = point.x; x < point.x + 15; ++x) {
			buffer[y * 120 + x] = self_idx;
		}
	}
}

void ElectrolyzerM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y - 1) * 120 + point.x + 7] = &m_input;
	buffer[(point.y + 1) * 120 + point.x + 5] = &m_output0;
	buffer[(point.y + 1) * 120 + point.x + 10] = &m_output1;
}

void ElectrolyzerM::InsertItem(Item given_item) {
	if (given_item == ITEM_WATER) {
		m_stored_count++;
	}
}

void ElectrolyzerM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	stats;

	if (m_stored_count > 0) {
		auto anchors0 = m_output0.m_piped_anchors;
		if (anchors0.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors0.size() - 1));
			Anchor* target = anchors0[dist(rng)];
			target->m_parent->InsertItem(ITEM_HYDROGEN);
		}

		auto anchors1 = m_output1.m_piped_anchors;
		if (anchors1.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors1.size() - 1));
			Anchor* target = anchors1[dist(rng)];
			target->m_parent->InsertItem(ITEM_OXYGEN);
		}

		m_stored_count--;
	}
}

// CUTTER MACHINE

CutterM::CutterM(Point point) : Machine(point), m_input(this), m_output(this), m_stored_count() {}

CutterM::~CutterM() {}

void CutterM::upgrade_anchors() {
	m_input.m_parent = this;
	m_output.m_parent = this;
}

bool CutterM::is_breakable() {
	return true;
}

string CutterM::get_name() {
	return "Cutter";
}

vector<Anchor*> CutterM::get_anchors() {
	return { &m_input, &m_output };
}

void CutterM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x, point.y, "[[Cutter]]");
	draw_manager->DrawLabel(point.x + 5, point.y - 1, "I");
	draw_manager->DrawLabel(point.x + 5, point.y + 1, "O");
}

void CutterM::CreateIdx(size_t* buffer, size_t self_idx) {
	for (int y = point.y; y < point.y + 1; ++y) {
		for (int x = point.x; x < point.x + 15; ++x) {
			buffer[y * 120 + x] = self_idx;
		}
	}
}

void CutterM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y - 1) * 120 + point.x + 5] = &m_input;
	buffer[(point.y + 1) * 120 + point.x + 5] = &m_output;
}

void CutterM::InsertItem(Item given_item) {
	if (given_item == ITEM_SILICON) {
		m_stored_count[0]++;
	}
	if (given_item == ITEM_CIRCUIT_WAFER) {
		m_stored_count[1]++;
	}
}

void CutterM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	stats;

	if (m_stored_count[0] > 0) {
		auto anchors0 = m_output.m_piped_anchors;
		if (anchors0.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors0.size() - 1));
			Anchor* target = anchors0[dist(rng)];
			target->m_parent->InsertItem(ITEM_SILICON_WAFER);
		}
		m_stored_count[0]--;
	}

	if (m_stored_count[1] > 0) {
		auto anchors0 = m_output.m_piped_anchors;
		if (anchors0.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors0.size() - 1));
			Anchor* target = anchors0[dist(rng)];
			target->m_parent->InsertItem(ITEM_CIRCUIT);
		}
		m_stored_count[1]--;
	}
}

// LAZER MACHINE

LaserM::LaserM(Point point) : Machine(point), m_input(this), m_output(this), m_stored_count(0) {}

LaserM::~LaserM() {}

void LaserM::upgrade_anchors() {
	m_input.m_parent = this;
	m_output.m_parent = this;
}

bool LaserM::is_breakable() {
	return true;
}

string LaserM::get_name() {
	return "Laser";
}

vector<Anchor*> LaserM::get_anchors() {
	return { &m_input, &m_output };
}

void LaserM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x, point.y, "[[Laser]]");
	draw_manager->DrawLabel(point.x + 5, point.y - 1, "I");
	draw_manager->DrawLabel(point.x + 5, point.y + 1, "O");
}

void LaserM::CreateIdx(size_t* buffer, size_t self_idx) {
	for (int y = point.y; y < point.y + 1; ++y) {
		for (int x = point.x; x < point.x + 15; ++x) {
			buffer[y * 120 + x] = self_idx;
		}
	}
}

void LaserM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y - 1) * 120 + point.x + 5] = &m_input;
	buffer[(point.y + 1) * 120 + point.x + 5] = &m_output;
}

void LaserM::InsertItem(Item given_item) {
	if (given_item == ITEM_SILICON_WAFER) {
		m_stored_count++;
	}
}

void LaserM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	stats;

	if (m_stored_count > 0) {
		auto anchors0 = m_output.m_piped_anchors;
		if (anchors0.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors0.size() - 1));
			Anchor* target = anchors0[dist(rng)];
			target->m_parent->InsertItem(ITEM_CIRCUIT_WAFER);
		}

		m_stored_count--;
	}
}

// ASSEMBLER MACHINE

AssemblerM::AssemblerM(Point point) : Machine(point), m_input0(this), m_input1(this), m_input2(this), m_output(this), m_stored_count() {}

AssemblerM::~AssemblerM() {}

void AssemblerM::upgrade_anchors() {
	m_input0.m_parent = this;
	m_input1.m_parent = this;
	m_input2.m_parent = this;
	m_output.m_parent = this;
}

bool AssemblerM::is_breakable() {
	return true;
}

string AssemblerM::get_name() {
	return "Assembler";
}

vector<Anchor*> AssemblerM::get_anchors() {
	return { &m_input0, &m_input1, &m_input2, &m_output };
}

void AssemblerM::Draw(DrawManager* draw_manager) {
	draw_manager->DrawLabel(point.x, point.y, "[[Assembler]]");
	draw_manager->DrawLabel(point.x + 2, point.y - 1, "I1");
	draw_manager->DrawLabel(point.x + 5, point.y - 1, "I2");
	draw_manager->DrawLabel(point.x + 8, point.y - 1, "I3");
	draw_manager->DrawLabel(point.x + 5, point.y + 1, "O");
}

void AssemblerM::CreateIdx(size_t* buffer, size_t self_idx) {
	for (int y = point.y; y < point.y + 1; ++y) {
		for (int x = point.x; x < point.x + 15; ++x) {
			buffer[y * 120 + x] = self_idx;
		}
	}
}

void AssemblerM::CreateAnchor(Anchor** buffer) {
	buffer[(point.y - 1) * 120 + point.x + 2] = &m_input0;
	buffer[(point.y - 1) * 120 + point.x + 5] = &m_input1;
	buffer[(point.y - 1) * 120 + point.x + 8] = &m_input2;
	buffer[(point.y + 1) * 120 + point.x + 5] = &m_output;
}

void AssemblerM::InsertItem(Item given_item) {
	if (given_item == ITEM_CIRCUIT) {
		m_stored_count[0]++;
	}
	if (given_item == ITEM_SOLDERING_IRON) {
		m_stored_count[1]++;
	}
	if (given_item == ITEM_CIRCUIT_BOARD) {
		m_stored_count[2]++;
	}
}

void AssemblerM::Evaluate(ProductiveStats* stats, default_random_engine rng) {
	stats;

	if (m_stored_count[0] > 0 && m_stored_count[1] > 0 && m_stored_count[2] > 0) {
		auto anchors0 = m_output.m_piped_anchors;
		if (anchors0.size() > 0) {
			uniform_int_distribution<> dist(0, static_cast<int>(anchors0.size() - 1));
			Anchor* target = anchors0[dist(rng)];
			target->m_parent->InsertItem(ITEM_CHIP);
		}
		m_stored_count[0]--;
		m_stored_count[1]--;
		m_stored_count[2]--;
	}
}

// MACHINE MANAGER

const size_t NULL_IDX = static_cast<size_t>(-1);

MachineManager::MachineManager() : m_machines(), m_sparse_idx(120 * 30), m_anchor_idx(120 * 30) {}

MachineManager::~MachineManager() {}

void MachineManager::AddMachine(shared_ptr<Machine> machine)
{
	m_machines.push_back(machine);
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
shared_ptr<Machine> MachineManager::RemoveMachine(Point point)
{
	for (size_t i = 0; i < 120 * 30; ++i) {
		m_sparse_idx[i] = NULL_IDX;
	}

	for (size_t i = 0; i < m_machines.size(); ++i) {
		shared_ptr<Machine> machine = m_machines[i];
		machine->CreateIdx(m_sparse_idx.data(), i);
	}

	size_t index_to_remove = m_sparse_idx[point.y * 120 + point.x];

	if (index_to_remove != static_cast<size_t>(-1) && m_machines[index_to_remove]->is_breakable()) {
		shared_ptr<Machine> machine = m_machines[index_to_remove];
		m_machines.erase(m_machines.begin() + index_to_remove);
		return machine;
	}

	return 0;
}

void MachineManager::Draw(DrawManager* draw_manager)
{
	for (size_t i = 0; i < m_machines.size(); ++i) {
		shared_ptr<Machine> machine = m_machines[i];
		machine->Draw(draw_manager);
	}
}

// Immediaelyでバッファに書き込んでいる、データ構造を考えたい
Anchor* MachineManager::GetAnchor(Point point)
{
	for (size_t i = 0; i < 120 * 30; ++i) {
		m_anchor_idx[i] = nullptr;
	}

	for (size_t i = 0; i < m_machines.size(); ++i) {
		shared_ptr<Machine> machine = m_machines[i];
		machine->CreateAnchor(m_anchor_idx.data());
	}

	return m_anchor_idx[point.y * 120 + point.x];
}

void MachineManager::Evaluate(ProductiveStats* stats, default_random_engine rng)
{
	for (size_t i = 0; i < m_machines.size(); ++i) {
		shared_ptr<Machine> machine = m_machines[i];
		machine->Evaluate(stats, rng);
	}
}