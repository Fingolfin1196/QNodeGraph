#include "NodeGraphScene.hpp"
#include "BackgroundItem.hpp"
#include "NodeItemGroup.hpp"
#include "NodeProxy.hpp"
#include "helpers/Conversions.hpp"
#include "helpers/Typing.hpp"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtDebug>

void NodeGraphScene::deleteItems(QList<QGraphicsItem*> q_items) {
  std::deque<NodeItemGroup*> nodes{};
  for (auto* item : q_items) {
    if (auto* cast_ptr{dynamic_cast<NodeItemGroup*>(item)})
      nodes.push_back(cast_ptr);
    else {
      removeItem(item);
      delete item;
    }
  }
  for (auto* node : nodes) {
    removeItem(node);
    delete node;
  }
}
void NodeGraphScene::clearItems() {
  for (auto* item : this->items())
    if (auto* cast_ptr{dynamic_cast<ConnectionItem*>(item)}) {
      removeItem(item);
      delete item;
    }
  clear();
}

QJsonObject NodeGraphScene::toJson() const {
  QJsonArray nodes{}, connections{};
  for (auto* item : items()) {
    auto* node_ptr{dynamic_cast<NodeItemGroup*>(item)};
    auto* conn_ptr{dynamic_cast<ConnectionItem*>(item)};
    assert(not node_ptr or not conn_ptr);
    if (not node_ptr and not conn_ptr) {
#ifdef NDEBUG
      continue;
#else
      std::cout << "other skipped: ";
      if (dynamic_cast<BackgroundItem*>(item)) {
        std::cout << "background!" << std::endl;
        continue;
      }
      if (dynamic_cast<NodeProxy*>(item)) {
        std::cout << "proxy!" << std::endl;
        continue;
      }
      if (dynamic_cast<PortItem<PortType::INPUT>*>(item)) {
        std::cout << "input port!" << std::endl;
        continue;
      }
      if (dynamic_cast<PortItem<PortType::OUTPUT>*>(item)) {
        std::cout << "output port!" << std::endl;
        continue;
      }
      std::cout << "unknown!" << std::endl;
      continue;
#endif
    }
    if (node_ptr)
      nodes.append(QJsonObject{
          {"type_id", node_ptr->typeIdentification()},
          {"data", node_ptr->toJson()},
          {"pos", convertToJson(node_ptr->pos())},
          {"transform", convertToJson(node_ptr->transform())},
      });
    if (conn_ptr)
      connections.append(QJsonObject{
          {"type_id", conn_ptr->typeIdentification()},
          {"data", conn_ptr->toJson()},
          {"pos", convertToJson(conn_ptr->pos())},
          {"transform", convertToJson(conn_ptr->transform())},
      });
  }
  return {{"nodes", nodes}, {"connections", connections}};
}

void NodeGraphScene::fromJson(QJsonObject json) {
  clearItems();
  PortStorage storage{};
  {
    PortRegistration registration{storage};
    assert(json.contains("nodes"));
    QJsonValue node_value{json.value("nodes")};
    assert(node_value.isArray());
    QJsonArray nodes(node_value.toArray());
    for (QJsonValue value : nodes) {
      assert(value.isObject());
      QJsonObject object{value.toObject()};
      assert(object.contains("type_id"));
      auto key{object.value("type_id").toString()};
      assert(node_registry_.find(key) != node_registry_.end());
      auto* node{node_registry_.at(key)(object.value("data").toObject(), registration)};
      node->setPos(convertFromJson<QPointF>(object.value("pos").toObject()));
      node->setTransform(convertFromJson<QTransform>(object.value("transform").toObject()));
      addItem(node);
    }
  }
  {
    QJsonArray connections(json.value("connections").toArray());
    for (QJsonValue value : connections) {
      QJsonObject object{value.toObject()};
      auto* connection(
          connection_registry_.at(object.value("type_id").toString())(object.value("data").toObject(), storage));
      connection->setTransform(convertFromJson<QTransform>(object.value("transform").toObject()));
      addItem(connection);
    }
  }
}
