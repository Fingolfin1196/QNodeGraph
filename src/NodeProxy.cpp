#include "NodeProxy.hpp"

QVBoxLayout* NodeProxy::makeLayout() const {
  auto* layout{new QVBoxLayout};
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(title_);
  if (central_) layout->addWidget(central_);
  layout->addWidget(status_);
  return layout;
}

void NodeProxy::updateState(std::optional<QString> message) {
  switch (state_) {
    case NodeState::NORMAL: setStatus(message.has_value() ? message.value() : "normal"); break;
    case NodeState::INVALID_ARGUMENTS:
      setStatus(message.has_value() ? "invalid state: " + message.value() : "invalid state");
      break;
    case NodeState::MISSING_INPUTS:
      setStatus(message.has_value() ? ("missing inputs: " + message.value()) : "missing inputs");
      break;
    case NodeState::INVALID_INPUTS:
      setStatus(message.has_value() ? ("invalid inputs: " + message.value()) : "invalid inputs");
      break;
  }
}
