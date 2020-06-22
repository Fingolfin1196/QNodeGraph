#pragma once
#include "NumberConnection.hpp"
#include "TypedPortItem.hpp"
#include "default/DefaultPortItem.hpp"
#include "default/UniformConnectionContainer.hpp"

namespace __detail {
template<PortType Dir, typename Num, typename Derived> class __NumberPort
    : public DefaultPortItem<Dir, UniformConnectionContainer<NumberConnection<Num>>, NumberConnection<Num>> {
public:
  using connection_t = NumberConnection<Num>;
  using data_t = UniformConnectionContainer<connection_t>;
  using parent_t = DefaultPortItem<Dir, data_t, NumberConnection<Num>>;

private:
  std::optional<Num> value_{};

protected:
  void setValue(std::optional<Num> new_value) { value_ = std::move(new_value); }

  __NumberPort(data_t data, QGraphicsItem* parent = nullptr) : parent_t(std::move(data), parent) {}

public:
  std::optional<Num> value() const { return value_; }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  __NumberPort(QGraphicsItem* parent = nullptr) : parent_t(parent) {}
  __NumberPort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t(json, parent) {}
};
} // namespace __detail

template<PortType Dir, typename Num> class NumberPort {};

template<typename Num> class NumberPort<PortType::INPUT, Num>
    : public __detail::__NumberPort<PortType::INPUT, Num, NumberPort<PortType::INPUT, Num>> {
  using parent_t = __detail::__NumberPort<PortType::INPUT, Num, NumberPort<PortType::INPUT, Num>>;

protected:
  NumberPort(typename parent_t::data_t data, QGraphicsItem* parent = nullptr) : parent_t(std::move(data), parent) {}

public:
  static inline QString typeID() {
    return QStringLiteral("NumberInputPort<") + typeString<Num>() + QStringLiteral(">");
  }

  virtual void preparePropaganda(){};
  virtual void acceptPropaganda(std::optional<Num>) = 0;

  NumberPort(QGraphicsItem* parent = nullptr) : parent_t(parent) {}
};
template<typename Num> class NumberPort<PortType::OUTPUT, Num>
    : public __detail::__NumberPort<PortType::OUTPUT, Num, NumberPort<PortType::OUTPUT, Num>> {
  using parent_t = __detail::__NumberPort<PortType::OUTPUT, Num, NumberPort<PortType::OUTPUT, Num>>;

protected:
  ConnectionItem* createConnection(QGraphicsSceneMouseEvent* event) override {
    auto* connection{new NumberConnection<Num>(this, event->scenePos(), this->view()->nodeGraphScene())};
    this->addConnection(connection);
    return connection;
  }

public:
  static inline QString typeID() {
    return QStringLiteral("NumberOutputPort<") + typeString<Num>() + QStringLiteral(">");
  }

  void updateValue(std::optional<Num> new_value) {
    if (new_value.has_value())
      std::cout << new_value.value() << std::endl;
    else
      std::cout << "invalid" << std::endl;
    this->setValue(std::move(new_value));
    this->preparePropagation();
    this->propagate();
  }

  template<typename... Args> NumberPort(Args&&... args) : parent_t(std::forward<Args>(args)...) {}
  NumberPort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t(json, parent) {}
};

namespace __detail {
template<PortType Dir, typename Num, typename Node, typename Derived> class __NodeNumberPort
    : public NumberPort<Dir, Num> {
  Node* node_;

protected:
  using parent_t = NumberPort<Dir, Num>;

  Node* typedNode() const { return node_; }
  void connectionAdded(NumberConnection<Num>* connection) override {
    if constexpr (Dir == PortType::INPUT) {
      this->setValue(connection->typedStartItem()->value());
      node_->inputConnectionAdded(static_cast<Derived*>(this));
    }
  }
  void connectionRemoved(NumberConnection<Num>*) override {
    if constexpr (Dir == PortType::INPUT) {
      this->setValue(std::nullopt);
      node_->inputConnectionRemoved(static_cast<Derived*>(this));
    }
  }

public:
  NodeItemGroup* node() const final { return typedNode(); }
  QPointF localCentre() const final { return node_->template portCentre<Dir>(static_cast<const Derived*>(this)); }
  qreal radius() const final { return node_->template portRadius<Dir>(static_cast<const Derived*>(this)); }

  template<typename... Args> __NodeNumberPort(Node* node, Args&&... args)
      : parent_t{std::forward<Args>(args)...}, node_{node} {}
  __NodeNumberPort(QJsonObject json, Node* node, QGraphicsItem* parent = nullptr)
      : __NodeNumberPort{node, json, parent} {}
};
} // namespace __detail

template<PortType Dir, typename Num, typename Node> class NodeNumberPort final
    : public __detail::__NodeNumberPort<Dir, Num, Node, NodeNumberPort<Dir, Num, Node>> {
protected:
  using parent_t = __detail::__NodeNumberPort<Dir, Num, Node, NodeNumberPort<Dir, Num, Node>>;

public:
  template<typename... Args> NodeNumberPort(Args&&... args) : parent_t(std::forward<Args>(args)...) {}
  NodeNumberPort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t(json, parent) {}
};
template<typename Num, typename Node> class NodeNumberPort<PortType::INPUT, Num, Node>
    : public __detail::__NodeNumberPort<PortType::INPUT, Num, Node, NodeNumberPort<PortType::INPUT, Num, Node>> {
protected:
  using parent_t = __detail::__NodeNumberPort<PortType::INPUT, Num, Node, NodeNumberPort<PortType::INPUT, Num, Node>>;

public:
  void acceptPropaganda(std::optional<Num> value) override {
    this->setValue(std::move(value));
    this->typedNode()->inputConnectionUpdated(this);
  }

  template<typename... Args> NodeNumberPort(Args&&... args) : parent_t(std::forward<Args>(args)...) {}
  NodeNumberPort(QJsonObject json, Node* node, QGraphicsItem* parent = nullptr) : parent_t(json, node, parent) {}
};
