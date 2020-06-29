#pragma once
#include "PortType.hpp"
#include <QtCore/QHash>
#include <QtCore/QString>
#include <unordered_map>

template<PortType> class PortItem;
class PortStorage {
  struct QStringHash {
    std::size_t operator()(const QString& string) const { return qHash(string); }
  };

  std::unordered_map<QString, PortItem<PortType::INPUT>*, QStringHash> input_port_registry_{};
  std::unordered_map<QString, PortItem<PortType::OUTPUT>*, QStringHash> output_port_registry_{};

public:
  void registerInputPort(const QString& id, PortItem<PortType::INPUT>* port) { input_port_registry_.emplace(id, port); }
  void registerOutputPort(const QString& id, PortItem<PortType::OUTPUT>* port) {
    output_port_registry_.emplace(id, port);
  }

  PortItem<PortType::INPUT>* retrieveInput(const QString& port_id) const { return input_port_registry_.at(port_id); }
  PortItem<PortType::OUTPUT>* retrieveOutput(const QString& port_id) const { return output_port_registry_.at(port_id); }
};

class PortRegistration {
  PortStorage& storage_;

public:
  void registerInputPort(const QString& id, PortItem<PortType::INPUT>* port) { storage_.registerInputPort(id, port); }
  void registerOutputPort(const QString& id, PortItem<PortType::OUTPUT>* port) {
    storage_.registerOutputPort(id, port);
  }
  template<PortType Dir> void registerPort(const QString& id, PortItem<Dir>* port) {
    if constexpr (Dir == PortType::INPUT)
      registerInputPort(id, port);
    else
      registerOutputPort(id, port);
  }

  PortRegistration(PortStorage& storage) : storage_{storage} {}
};
