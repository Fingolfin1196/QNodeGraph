#pragma once
#include "../LoadRegistry.hpp"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

template<typename Node, typename Port> class UniformPortContainer : public std::vector<Port*> {
  using ports_t = std::vector<Port*>;

  Node* node_;

  static ports_t generatePorts(const std::size_t count, Node* node) {
    ports_t ports(count);
    for (std::size_t i{0}; i < count; ++i) ports[i] = new Port{node};
    return ports;
  }
  static ports_t generatePorts(QJsonArray json, PortRegistration& registration, Node* node) {
    ports_t ports(json.size());
    std::transform(json.begin(), json.end(), ports.begin(), [node, &registration](QJsonValue value) {
      auto json_object{value.toObject()};
      auto* port{new Port{json_object.value("data").toObject(), node}};
      registration.registerPort(json_object.value("address").toString(), port);
      return port;
    });
    return ports;
  }

public:
  std::size_t indexOf(const Port* port) const {
    auto it{std::find(this->begin(), this->end(), port)};
    assert(it != this->end());
    return it - this->begin();
  }

  QJsonObject toJson() const {
    QJsonArray array{};
    for (auto* port : *this) array.append(QJsonObject{{"address", ptrToStr(port)}, {"data", port->toJson()}});
    return {{"ports", array}};
  }

  UniformPortContainer(const std::size_t count, Node* node) : node_{node}, ports_t{generatePorts(count, node)} {}
  UniformPortContainer(QJsonObject json, PortRegistration& registration, Node* node)
      : node_{node}, ports_t{generatePorts(json.value("ports").toArray(), registration, node)} {}
};