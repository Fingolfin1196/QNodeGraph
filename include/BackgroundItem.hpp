#pragma once
#include "NodeState.hpp"
#include "helpers/Typing.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QGraphicsItem>
#include <sstream>

class BackgroundItem : public QGraphicsItem {
  NodeState state_;

public:
  NodeState state() const { return state_; }
  void setState(NodeState state) { state_ = state; }

  virtual void preGeometryChange() { prepareGeometryChange(); }

  virtual QJsonObject toJson() const { return {{"state", int{to_underlying(state_)}}}; }

  BackgroundItem(NodeState state, QGraphicsItem* parent = nullptr) : QGraphicsItem{parent}, state_{state} {}
  BackgroundItem(QJsonObject json, QGraphicsItem* parent = nullptr)
      : BackgroundItem{from_underlying<NodeState>(json.value("state").toInt()), parent} {}
};