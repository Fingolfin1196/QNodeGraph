#include "ConnectionItem.hpp"
#include "NodeItemGroup.hpp"

template<PortType type> inline auto* portItem(QPointF scene_pos, NodeGraphScene* scene, ConnectionItem* connection) {
  return scene->isTypeHit<PortItem<type>>(scene_pos, [connection](PortItem<type>& port) {
    return port.portType() == type and port.isConnectionAccepted(connection);
  });
}

bool ConnectionItem::mouseRelease(QPointF scene_pos) {
  const bool requires_input{not endItem()}, requires_output{not startItem()};
  assert(not requires_input or not requires_output);
  if (not requires_input and not requires_output) return false;
  if (requires_input) {
    auto* port{portItem<PortType::INPUT>(scene_pos, scene_, this)};
    if (not port) {
      scene_->removeItem(this);
      deleteLater();
    } else
      setEndPort(port);
  } else {
    auto* port{portItem<PortType::OUTPUT>(scene_pos, scene_, this)};
    if (not port) {
      scene_->removeItem(this);
      deleteLater();
    } else
      setStartPort(port);
  }
  return true;
}

void ConnectionItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  auto* start{startItem()};
  auto* end{endItem()};
  assert(start);
  const QPointF pos{event->scenePos()};
  if (end) {
    if (end->contains(end->mapFromScene(pos))) {
      prepareGeometryChange();
      QGraphicsItem::mousePressEvent(event);
      removeEndItem();
      temp_ = pos;
    } else if (start->contains(start->mapFromScene(pos))) {
      event->ignore();
    } else
      QGraphicsItem::mousePressEvent(event);
  } else {
    QGraphicsItem::mousePressEvent(event);
    temp_ = pos;
  }
}
void ConnectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (not mouseRelease(event->scenePos())) return QGraphicsItem::mouseReleaseEvent(event);
}
QVariant ConnectionItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) {
  if (change == GraphicsItemChange::ItemSceneChange and not value.value<QGraphicsScene*>()) unlink();
  return QGraphicsItem::itemChange(change, value);
}

QPointF ConnectionItem::startPoint() const {
  auto* start{startItem()};
  auto* end{endItem()};
  assert(start or end);
  return start ? start->sceneCentre() : temp_;
}
QPointF ConnectionItem::endPoint() const {
  auto* start{startItem()};
  auto* end{endItem()};
  assert(start or end);
  return end ? end->sceneCentre() : temp_;
}

void ConnectionItem::setStartPort(PortItem<PortType::OUTPUT>* port) {
  auto* start{startItem()};
  auto* end{endItem()};
  assert(not start and end);
  prepareGeometryChange();
  setStartItem(port);
}
void ConnectionItem::setEndPort(PortItem<PortType::INPUT>* port) {
  auto* start{startItem()};
  auto* end{endItem()};
  assert(start and not end);
  prepareGeometryChange();
  setEndItem(port);
}

void ConnectionItem::unlink() {
  auto* start{startItem()};
  auto* end{endItem()};
  if (start) start->removeConnection(this);
  if (end) end->removeConnection(this);
}
