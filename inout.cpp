#include "inout.h"

Input::Input(Point point, Item item) : point(point), m_item(item) {}

Input::~Input() {}

string Input::get_name() {
	return item_to_string(m_item);
}

void Input::Draw(DrawManager* draw_manager) {
	string name = get_name();
	draw_manager->DrawLabel(point.x, point.y, name);
	draw_manager->DrawLabel(point.x + (name.size() / 2), point.y + 1, "I");
}

void Input::CreateAnchor(Anchor** buffer) {
	string name = get_name();
	buffer[(point.y + 1) * 120 + (point.x + name.size() / 2)] = new Anchor(nullptr);
}

Output::Output(Point point, Item item) : point(point), m_item(item) {}

Output::~Output() {}

InoutManager::InoutManager() : m_inputs(), m_outputs(), m_anchor_idx(120 * 30, nullptr) {}

InoutManager::~InoutManager() {}

void InoutManager::Draw(DrawManager* draw_manager) {
	for (size_t i = 0; i < m_inputs.size(); ++i) {
		m_inputs[i]->Draw(draw_manager);
	}
	for (size_t i = 0; i < m_outputs.size(); ++i) {
		m_outputs[i]->Draw(draw_manager);
	}
}

void InoutManager::AddInput(shared_ptr<Input> input) {
	m_inputs.push_back(input);
}

void InoutManager::AddOutput(shared_ptr<Output> output) {
	m_outputs.push_back(output);
}

Anchor* InoutManager::GetAnchor(Point point) {
	for (size_t i = 0; i < 120 * 30; ++i) {
		m_anchor_idx[i] = nullptr;
	}
	for (size_t i = 0; i < m_inputs.size(); ++i) {
		m_inputs[i]->CreateAnchor(m_anchor_idx.data());
	}
	for (size_t i = 0; i < m_outputs.size(); ++i) {
		m_outputs[i]->CreateAnchor(m_anchor_idx.data());
	}
	return m_anchor_idx[point.y * 120 + point.x];
}
