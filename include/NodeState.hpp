#pragma once
#include <sstream>

enum class NodeState { NORMAL, INVALID_ARGUMENTS, MISSING_INPUTS, INVALID_INPUTS };
static inline std::ostream& operator<<(std::ostream& stream, NodeState state) {
  switch (state) {
    case NodeState::NORMAL: return stream << "NORMAL";
    case NodeState::INVALID_ARGUMENTS: return stream << "INVALID_ARGUMENTS";
    case NodeState::MISSING_INPUTS: return stream << "MISSING_INPUTS";
    case NodeState::INVALID_INPUTS: return stream << "INVALID_INPUTS";
  }
  throw std::invalid_argument("The state cannot be handled!");
}
