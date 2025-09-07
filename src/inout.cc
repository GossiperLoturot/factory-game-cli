#include "inout.h"

#include "machine.h"

namespace factory_game {

Input::Input(Point point, Item item) : point(point), m_item(item) {}

Input::~Input() = default;

std::string Input::get_name() { return item_to_string(m_item); }

void Input::draw(DrawManager* draw_manager) {
  std::string name = get_name();
  draw_manager->draw_label(point.x, point.y, name);
  draw_manager->draw_label(point.x + (name.size() / 2), point.y + 1, "I");
}

void Input::create_anchor(Anchor** buffer) {
  std::string name = get_name();
  buffer[(point.y + 1) * 120 + (point.x + name.size() / 2)] =
      new Anchor(nullptr);
}

Output::Output(Point point, Item item) : point(point), m_item(item) {}

Output::~Output() = default;

InoutManager::InoutManager()
    : m_inputs(), m_outputs(), m_anchor_idx(120 * 30, nullptr) {}

InoutManager::~InoutManager() = default;

void InoutManager::draw(DrawManager* draw_manager) {
  for (size_t i = 0; i < m_inputs.size(); ++i) {
    m_inputs[i]->draw(draw_manager);
  }
  for (size_t i = 0; i < m_outputs.size(); ++i) {
    m_outputs[i]->draw(draw_manager);
  }
}

void InoutManager::add_input(std::shared_ptr<Input> input) {
  m_inputs.push_back(input);
}

void InoutManager::add_output(std::shared_ptr<Output> output) {
  m_outputs.push_back(output);
}

Anchor* InoutManager::get_anchor(Point point) {
  for (size_t i = 0; i < 120 * 30; ++i) {
    m_anchor_idx[i] = nullptr;
  }
  for (size_t i = 0; i < m_inputs.size(); ++i) {
    m_inputs[i]->create_anchor(m_anchor_idx.data());
  }
  for (size_t i = 0; i < m_outputs.size(); ++i) {
    m_outputs[i]->create_anchor(m_anchor_idx.data());
  }
  return m_anchor_idx[point.y * 120 + point.x];
}

}  // namespace factory_game