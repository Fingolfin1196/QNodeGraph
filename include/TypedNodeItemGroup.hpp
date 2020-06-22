#pragma once
#include "LoadRegistry.hpp"
#include "NodeItemGroup.hpp"
#include "helpers/Interval.hpp"
#include "helpers/Typing.hpp"
#include <sstream>

template<typename Widget, typename Background, typename InPorts, typename OutPorts, typename View, typename Derived>
class TypedNodeItemGroup : public NodeItemGroup {
  View* view_;
  Widget* widget_;
  Background* background_;
  InPorts in_ports_;
  OutPorts out_ports_;

protected:
  Widget* innerWidget() const { return widget_; }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
    for (auto* port : in_ports_) port->preGeometryChange();
    for (auto* port : out_ports_) port->preGeometryChange();
    QGraphicsItemGroup::mouseMoveEvent(event);
  }

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override {
    if (change == GraphicsItemChange::ItemSceneChange and not value.value<QGraphicsScene*>()) {
      for (auto* port : in_ports_) port->removeConnections();
      for (auto* port : out_ports_) port->removeConnections();
    }
    return QGraphicsItemGroup::itemChange(change, value);
  }

  template<typename... Args> TypedNodeItemGroup(Widget* widget, Background* background, InPorts in_ports,
                                                OutPorts out_ports, View* view, Args&&... args)
      : NodeItemGroup(std::forward<Args>(args)...), view_{view}, widget_{widget},
        background_{background}, in_ports_{in_ports}, out_ports_{out_ports} {
    background_->setParentItem(this), addToGroup(background_);
    for (auto* port : in_ports_) port->setParentItem(this), addToGroup(port);
    for (auto* port : out_ports_) port->setParentItem(this), addToGroup(port);
    widget_->setParentItem(this), addToGroup(widget_);
  }

public:
  std::size_t inputNumber() const { return in_ports_.size(); }
  std::size_t outputNumber() const { return out_ports_.size(); }

  void preGeometryChange() override {
    widget_->preGeometryChange();
    background_->preGeometryChange();
    for (auto* port : in_ports_) port->preGeometryChange();
    for (auto* port : out_ports_) port->preGeometryChange();
    NodeItemGroup::prepareGeometryChange();
  }

  void setState(NodeState state, std::optional<QString> message = std::nullopt) {
    prepareGeometryChange();
    background_->setState(state);
    widget_->setState(state, std::move(message));
  }
  void setTitle(QString title) {
    prepareGeometryChange();
    widget_->setTitle(std::move(title));
  }

  template<PortType Dir, typename Port> qreal portRadius(Port* port) const {
    static_assert(Dir == PortType::INPUT or Dir == PortType::OUTPUT);
    if constexpr (Dir == PortType::INPUT)
      return background_->template portRadius<Dir>(in_ports_.indexOf(port));
    else
      return background_->template portRadius<Dir>(out_ports_.indexOf(port));
  }
  template<PortType Dir, typename Port> QPointF portCentre(Port* port) const {
    static_assert(Dir == PortType::INPUT or Dir == PortType::OUTPUT);
    if constexpr (Dir == PortType::INPUT)
      return background_->template portCentre<Dir>(in_ports_.indexOf(port));
    else
      return background_->template portCentre<Dir>(out_ports_.indexOf(port));
  }

  /**
   * @brief This seems to be necessary to prevent much too large bounding boxes or missing updates!
   *
   * @return QRectF The bounding box of the children.
   */
  QRectF boundingRect() const override {
    QRectF bounding{background_->boundingRect()};
    for (auto* port : in_ports_) bounding = bounding.united(port->boundingRect());
    for (auto* port : out_ports_) bounding = bounding.united(port->boundingRect());
    return bounding;
  }

  View* typedView() const { return view_; }
  NodeGraphView* view() const final { return typedView(); }

  QRectF widgetRect() const {
    auto rect{widget_->boundingRect()};
    rect.moveTo(widget_->pos());
    return rect;
  }

  const InPorts& inPorts() const { return in_ports_; }
  const OutPorts& outPorts() const { return out_ports_; }

  QJsonObject toJson() const override {
    return {{"widget", widget_->toJson()},
            {"background", background_->toJson()},
            {"in_ports", in_ports_.toJson()},
            {"out_ports", out_ports_.toJson()}};
  }
  QString typeIdentification() const final { return Derived::typeID(); }

  TypedNodeItemGroup(Widget* widget, NodeState state, std::size_t initial_inputs, std::size_t initial_outputs,
                     View* view, QGraphicsItem* parent = nullptr)
      : TypedNodeItemGroup{widget,
                           new Background{state, static_cast<Derived*>(this)},
                           {initial_inputs, static_cast<Derived*>(this)},
                           {initial_outputs, static_cast<Derived*>(this)},
                           view,
                           parent} {}
  TypedNodeItemGroup(NodeState state, std::size_t initial_inputs, std::size_t initial_outputs, View* view,
                     QGraphicsItem* parent = nullptr)
      : TypedNodeItemGroup{new Widget{state}, state, initial_inputs, initial_outputs, view, parent} {}
  TypedNodeItemGroup(QJsonObject json, PortRegistration& registration, View* view, QGraphicsItem* parent = nullptr)
      : TypedNodeItemGroup{new Widget{json.value("widget").toObject()},
                           new Background{json.value("background").toObject(), static_cast<Derived*>(this)},
                           {json.value("in_ports").toObject(), registration, static_cast<Derived*>(this)},
                           {json.value("out_ports").toObject(), registration, static_cast<Derived*>(this)},
                           view,
                           parent} {}
};