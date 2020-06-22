#pragma once
#include "ConnectionItem.hpp"
#include "PortType.hpp"
#include <QtWidgets/QGraphicsObject>

class NodeItemGroup;
class NodeGraphView;

namespace __detail {
NodeGraphView* view(NodeItemGroup* node);

template<PortType Dir> class __PortItem : public QGraphicsItem {
public:
  virtual NodeItemGroup* node() const = 0;
  NodeGraphView* view() const { return __detail::view(node()); }
  PortType portType() const { return Dir; }

  virtual QPointF localCentre() const = 0;
  QPointF sceneCentre() const { return mapToScene(localCentre()); }

  virtual void addConnection(ConnectionItem*) = 0;
  virtual void removeConnection(ConnectionItem*) = 0;
  virtual bool connectionsEmpty() const = 0;
  virtual void removeConnections() = 0;

  virtual void preGeometryChange() { prepareGeometryChange(); }

  virtual QJsonObject toJson() const { return {}; }

  __PortItem(QGraphicsItem* parent = nullptr) : QGraphicsItem{parent} {}
  __PortItem(QJsonObject, QGraphicsItem* parent = nullptr) : __PortItem{parent} {}
};
} // namespace __detail

template<PortType Dir> class PortItem {};
template<> class PortItem<PortType::OUTPUT> : public __detail::__PortItem<PortType::OUTPUT> {
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event);

public:
  virtual bool isConnectionAccepted(ConnectionItem* connection) const {
    return connection->startItem()->node() != node();
  }
  virtual ConnectionItem* createConnection(QGraphicsSceneMouseEvent* event) = 0;

  PortItem(QGraphicsItem* parent = nullptr) : __detail::__PortItem<PortType::OUTPUT>{parent} {}
  PortItem(QJsonObject json, QGraphicsItem* parent = nullptr) : __detail::__PortItem<PortType::OUTPUT>{json, parent} {}
};
template<> class PortItem<PortType::INPUT> : public __detail::__PortItem<PortType::INPUT> {
public:
  virtual bool isConnectionAccepted(ConnectionItem* connection) const {
    return connection->startItem()->node() != node() and connectionsEmpty();
  }

  PortItem(QGraphicsItem* parent = nullptr) : __detail::__PortItem<PortType::INPUT>{parent} {}
  PortItem(QJsonObject json, QGraphicsItem* parent = nullptr) : __detail::__PortItem<PortType::INPUT>{json, parent} {}
};
