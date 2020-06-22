#pragma once
#include "LoadRegistry.hpp"
#include "PortType.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <deque>

class NodeItemGroup;
class ConnectionItem;
template<PortType Type> class PortItem;
class NodeGraphScene : public QGraphicsScene {
  Q_OBJECT

public:
  using node_creator_t = std::function<NodeItemGroup*(QJsonObject, PortRegistration&)>;
  using connection_creator_t = std::function<ConnectionItem*(QJsonObject, const PortStorage&)>;
  using nodes_t = std::unordered_map<QString, node_creator_t>;
  using connections_t = std::unordered_map<QString, connection_creator_t>;

private:
  nodes_t node_registry_{};
  connections_t connection_registry_{};

  void deleteItems(QList<QGraphicsItem*> items);

public slots:
  void deleteSelection() { deleteItems(selectedItems()); }

public:
  template<typename T, typename P> T* isTypeHit(const P& element, std::function<bool(T&)> test) const {
    for (const auto& item : this->items(element)) {
      auto* object{dynamic_cast<T*>(item)};
      if (object and test(*object)) return object;
    }
    return nullptr;
  }
  template<typename T, typename P> T* isTypeHit(const P& element) const {
    for (const auto& item : this->items(element))
      if (auto* object{dynamic_cast<T*>(item)}) return object;
    return nullptr;
  }

  void registerNodeType(const QString& type_id, node_creator_t creator) {
    node_registry_.emplace(std::move(type_id), std::move(creator));
  }
  void registerConnectionType(const QString& type_id, connection_creator_t creator) {
    connection_registry_.emplace(std::move(type_id), std::move(creator));
  }

  void clearItems();

  void fromJson(QJsonObject json);
  QJsonObject toJson() const;

  explicit NodeGraphScene(QObject* parent = nullptr) : QGraphicsScene(parent) {
    // This is a nasty bit of business that solves the memory access issues by not indexing the elements.
    // The fix certainly works, but makes locating elements much slower in the process.
    setItemIndexMethod(QGraphicsScene::ItemIndexMethod::NoIndex);
  }
  ~NodeGraphScene() { clearItems(); }
};
