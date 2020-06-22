#pragma once
#include "NodeProxy.hpp"
#include "NumberPort.hpp"
#include "TypedNodeItemGroup.hpp"
#include "Typing.hpp"
#include "default/DefaultBackgroundItem.hpp"
#include "default/UniformPortContainer.hpp"
#include <QtWidgets/QLabel>
#include <optional>

class NumberGraphView;

namespace __detail {
template<typename Num> class NumberOutputProxy : public NodeProxy {
  QLabel* label_;

  void init() {
    label_->setAlignment(Qt::AlignCenter);
    setNoConnection();

    auto* layout{new QHBoxLayout};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label_);

    auto* wrapper{new QWidget};
    wrapper->setLayout(layout);
    setCentralWidget(wrapper);
  }

public:
  void setValue(std::optional<Num> value) {
    if (value.has_value())
      label_->setText(QString::number(value.value()));
    else
      label_->setText("no value");
  }
  void setNoConnection() { label_->setText("no value"); }

  QJsonObject toJson() const override { return {{"parent", NodeProxy::toJson()}}; }

  NumberOutputProxy(NodeState state, QWidget* widget_parent = nullptr, QGraphicsItem* item_parent = nullptr,
                    Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{state, item_parent, flags}, label_{new QLabel(widget_parent)} {
    init();
  }
  NumberOutputProxy(QJsonObject json, QWidget* widget_parent = nullptr, QGraphicsItem* item_parent = nullptr,
                    Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{json.value("parent").toObject(), item_parent, flags}, label_{new QLabel(widget_parent)} {
    init();
  }
};
} // namespace __detail

template<typename Num> class NumberOutputNode
    : public TypedNodeItemGroup<
          __detail::NumberOutputProxy<Num>, DefaultBackgroundItem<NumberOutputNode<Num>>,
          UniformPortContainer<NumberOutputNode<Num>, NodeNumberPort<PortType::INPUT, Num, NumberOutputNode<Num>>>,
          UniformPortContainer<NumberOutputNode<Num>, NodeNumberPort<PortType::OUTPUT, Num, NumberOutputNode<Num>>>,
          NumberGraphView, NumberOutputNode<Num>> {
  using in_port_t = NodeNumberPort<PortType::INPUT, Num, NumberOutputNode<Num>>;
  using out_port_t = NodeNumberPort<PortType::OUTPUT, Num, NumberOutputNode<Num>>;
  using parent_t =
      TypedNodeItemGroup<__detail::NumberOutputProxy<Num>, DefaultBackgroundItem<NumberOutputNode>,
                         UniformPortContainer<NumberOutputNode, in_port_t>,
                         UniformPortContainer<NumberOutputNode, out_port_t>, NumberGraphView, NumberOutputNode>;

public:
  static inline QString typeID() {
    return QStringLiteral("NumberOutputNode<") + typeString<Num>() + QStringLiteral(">");
  }

  void inputConnectionAdded(in_port_t* port) { inputConnectionUpdated(port); }
  void inputConnectionRemoved(in_port_t* port) {
    assert(port->node() == this);
    this->innerWidget()->setNoConnection();
    this->setState(NodeState::MISSING_INPUTS);
  }
  void inputConnectionUpdated(in_port_t* port) {
    assert(port->node() == this);
    auto value{port->value()};
    this->innerWidget()->setValue(value);
    this->setState(value ? NodeState::NORMAL : NodeState::INVALID_INPUTS);
  }
  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  NumberOutputNode(NumberGraphView* view, QGraphicsItem* parent = nullptr)
      : parent_t(NodeState::MISSING_INPUTS, 1, 0, view, parent) {
    this->setTitle(readableString<Num>() + " output");
  }
  NumberOutputNode(QJsonObject json, PortRegistration& registration, NumberGraphView* view,
                   QGraphicsItem* parent = nullptr)
      : parent_t(json.value("parent").toObject(), registration, view, parent) {}
};
