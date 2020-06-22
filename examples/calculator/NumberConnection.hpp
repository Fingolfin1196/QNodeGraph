#pragma once
#include "NodeGraphScene.hpp"
#include "TypedConnectionItem.hpp"
#include "Typing.hpp"
#include "default/DefaultConnectionItem.hpp"
#include <QtCore/QPointF>
#include <sstream>

template<PortType Dir, typename Num> class NumberPort;
template<typename Num> class NumberConnection final
    : public DefaultConnectionItem<NumberPort<PortType::OUTPUT, Num>, NumberPort<PortType::INPUT, Num>,
                                   NumberConnection<Num>> {
  using start_t = NumberPort<PortType::OUTPUT, Num>;
  using end_t = NumberPort<PortType::INPUT, Num>;
  using parent_t = DefaultConnectionItem<start_t, end_t, NumberConnection>;

  NumberConnection(start_t* start, end_t* end, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{start, end, temp_end, scene, parent} {}

public:
  static inline QString typeID() {
    return QStringLiteral("NumberConnection<") + typeString<Num>() + QStringLiteral(">");
  }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  NumberConnection(start_t* start, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t(start, temp_end, scene, parent) {}
  NumberConnection(QJsonObject json, const PortStorage& ports, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), ports, scene, parent} {}
};
