#pragma once
#include "NodeGraphView.hpp"
#include "NodeState.hpp"
#include "PortItem.hpp"
#include <QtCore/QJsonObject>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <cmath>
#include <iostream>

class NodeItemGroup : public QGraphicsItemGroup {
protected:
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override {}

public:
  virtual NodeGraphView* view() const = 0;

  virtual QJsonObject toJson() const { return {}; }
  virtual QString typeIdentification() const { return "NodeItemGroup"; }

  virtual void preGeometryChange() { prepareGeometryChange(); }

  NodeItemGroup(QGraphicsItem* parent = nullptr) : QGraphicsItemGroup{parent} {
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setHandlesChildEvents(false);
  }
  NodeItemGroup(QJsonObject, QGraphicsItem* parent = nullptr) : NodeItemGroup{parent} {}
};