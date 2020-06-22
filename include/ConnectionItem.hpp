#pragma once
#include "NodeGraphScene.hpp"
#include "PortType.hpp"
#include "helpers/Conversions.hpp"
#include "helpers/Typing.hpp"
#include <QtCore/QJsonObject>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QPen>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <iostream>

template<PortType Type> class PortItem;
class ConnectionItem : public QGraphicsObject {
  Q_OBJECT

  friend class NodeGraphView;

  NodeGraphScene* scene_;
  QPointF temp_;

  void setStartPort(PortItem<PortType::OUTPUT>* port);
  void setEndPort(PortItem<PortType::INPUT>* port);
  void unlink();

protected:
  QPointF startPoint() const;
  QPointF endPoint() const;

  void mouseMove(QPointF scene_pos) {
    temp_ = scene_pos;
    prepareGeometryChange();
  }
  bool mouseRelease(QPointF scene_pos);

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override { mouseMove(event->pos()); }
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;

public:
  /**
   * This function is expected to notify the previous start port of the change.
   * @param start A new port.
   */
  virtual void setStartItem(PortItem<PortType::OUTPUT>* start) = 0;
  /**
   * This function is expected to notify the previous end port of the change.
   * @param end A new port.
   */
  virtual void setEndItem(PortItem<PortType::INPUT>* end) = 0;
  virtual void removeEndItem() = 0;
  virtual PortItem<PortType::OUTPUT>* startItem() const = 0;
  virtual PortItem<PortType::INPUT>* endItem() const = 0;

  virtual void preGeometryChange() { prepareGeometryChange(); }

  virtual QJsonObject toJson() const { return {{"temp", convertToJson(temp_)}}; }
  virtual QString typeIdentification() const { return "ConnectionItem"; }

  ConnectionItem(QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : QGraphicsObject(parent), temp_{temp_end}, scene_{scene} {
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(1);
  }
  ConnectionItem(QJsonObject json, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : ConnectionItem{convertFromJson<QPointF>(json.value("temp").toObject()), scene, parent} {}
};