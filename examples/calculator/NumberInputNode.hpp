#pragma once
#include "NodeProxy.hpp"
#include "NodeState.hpp"
#include "NumberPort.hpp"
#include "TypedLineEdit.hpp"
#include "TypedNodeItemGroup.hpp"
#include "Typing.hpp"
#include "default/DefaultBackgroundItem.hpp"
#include "default/UniformPortContainer.hpp"
#include <QtWidgets/QHBoxLayout>

class NumberGraphView;

namespace __detail {
template<typename Num, typename Port> class NumberLineEdit final : public TypedLineEdit<Num> {
  Port* port_;

protected:
  void validValue(Num value) override {
    if (port_) port_->updateValue(value);
  }
  void invalidValue() override {
    if (port_) port_->updateValue(std::nullopt);
  }

public:
  void setPort(Port* port) {
    port_ = port;
    this->updateValue();
  }
  QJsonObject toJson() const override { return {{"parent", TypedLineEdit<Num>::toJson()}}; }

  explicit NumberLineEdit(QWidget* parent = nullptr) : TypedLineEdit<Num>(parent) {}
  explicit NumberLineEdit(QJsonObject json, QWidget* parent = nullptr) : TypedLineEdit<Num>(json, parent) {}
};

template<typename Num, typename Port> class NumberInputProxy : public NodeProxy {
  NumberLineEdit<Num, Port>* line_edit_;

  void init() {
    auto* layout{new QHBoxLayout};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(line_edit_);
    auto* wrapper{new QWidget};
    wrapper->setLayout(layout);
    setCentralWidget(wrapper);
  }

  NumberInputProxy(NodeState state, NumberLineEdit<Num, Port>* line_edit, QGraphicsItem* item_parent = nullptr,
                   Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{state, item_parent, flags}, line_edit_{line_edit} {
    init();
  }

public:
  void setPort(Port* port) { line_edit_->setPort(port); }
  NumberLineEdit<Num, Port>* lineEdit() const { return line_edit_; }
  QJsonObject toJson() const override { return {{"parent", NodeProxy::toJson()}, {"line_edit", line_edit_->toJson()}}; }

  NumberInputProxy(NodeState state, QWidget* widget_parent = nullptr, QGraphicsItem* item_parent = nullptr,
                   Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{state, item_parent, flags}, line_edit_{new NumberLineEdit<Num, Port>(widget_parent)} {
    init();
  }
  NumberInputProxy(QJsonObject json, QWidget* widget_parent = nullptr, QGraphicsItem* item_parent = nullptr,
                   Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{json, item_parent, flags}, line_edit_{new NumberLineEdit<Num, Port>(json, widget_parent)} {
    init();
  }
};
} // namespace __detail

template<typename Num> class NumberInputNode
    : public TypedNodeItemGroup<
          __detail::NumberInputProxy<Num, NodeNumberPort<PortType::OUTPUT, Num, NumberInputNode<Num>>>,
          DefaultBackgroundItem<NumberInputNode<Num>>,
          UniformPortContainer<NumberInputNode<Num>, NodeNumberPort<PortType::INPUT, Num, NumberInputNode<Num>>>,
          UniformPortContainer<NumberInputNode<Num>, NodeNumberPort<PortType::OUTPUT, Num, NumberInputNode<Num>>>,
          NumberGraphView, NumberInputNode<Num>> {
  using in_port_t = NodeNumberPort<PortType::INPUT, Num, NumberInputNode>;
  using out_port_t = NodeNumberPort<PortType::OUTPUT, Num, NumberInputNode>;
  using parent_t =
      TypedNodeItemGroup<__detail::NumberInputProxy<Num, out_port_t>, DefaultBackgroundItem<NumberInputNode>,
                         UniformPortContainer<NumberInputNode, in_port_t>,
                         UniformPortContainer<NumberInputNode, out_port_t>, NumberGraphView, NumberInputNode>;

public:
  static inline QString typeID() {
    return QStringLiteral("NumberInputNode<") + typeString<Num>() + QStringLiteral(">");
  }

  void inputConnectionUpdated(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }
  void inputConnectionAdded(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }
  void inputConnectionRemoved(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  NumberInputNode(NumberGraphView* view, QGraphicsItem* parent = nullptr)
      : parent_t(NodeState::NORMAL, 0, 1, view, parent) {
    this->innerWidget()->setPort(this->outPorts().at(0));
    this->setTitle(readableString<Num>() + " input");
  }
  NumberInputNode(QJsonObject json, PortRegistration& registration, NumberGraphView* view,
                  QGraphicsItem* parent = nullptr)
      : parent_t(json.value("parent").toObject(), registration, view, parent) {
    this->innerWidget()->setPort(this->outPorts().at(0));
  }
};
