#pragma once
#include "PortItem.hpp"

/**
 * @brief A typed version of PortItem that knows the type of valid connections and stores them.
 *
 * @tparam Dir Whether this is an input or output port.
 * @tparam Data The container in which to store connections.
 * @tparam Connection The type of a valid connection, which is used (amongst other things) for accepting connections.
 */
template<PortType Dir, typename Data, typename Connection> class TypedPortItem : public PortItem<Dir> {
  Data connections_;

protected:
  virtual void connectionAdded(Connection* connection) = 0;
  virtual void connectionRemoved(Connection* connection) = 0;

  void preparePropagation() {
    for (auto* connection : connections_) connection->preparePropagation();
  }
  void propagate() {
    for (auto* connection : connections_) connection->propagate();
  }

  template<typename... Args> TypedPortItem(Data data, Args&&... args)
      : PortItem<Dir>{std::forward<Args>(args)...}, connections_{std::move(data)} {}

public:
  virtual void addTypedConnection(Connection* connection) {
    assert(connection);
    connections_.insert(connection);
    connectionAdded(connection);
  }
  /**
   * @brief Add a connection. As we assume that only accepted connections are added, we can use a static_cast;
   *        this is somewhat dangerous, but more efficient.
   *
   * @param connection The connection to be added.
   */
  void addConnection(ConnectionItem* connection) override {
    assert(dynamic_cast<Connection*>(connection));
    addTypedConnection(static_cast<Connection*>(connection));
  }
  void removeConnection(ConnectionItem* connection) override {
    assert(dynamic_cast<Connection*>(connection));
    auto* cast_ptr{static_cast<Connection*>(connection)};
    connections_.remove(cast_ptr);
    connectionRemoved(cast_ptr);
  }
  bool connectionsEmpty() const override { return connections_.empty(); }
  void removeConnections() override {
    QGraphicsScene* scene{this->view()->scene()};
    for (auto connection : connections_) {
      scene->removeItem(connection);
      delete connection;
    }
    connections_.clear();
  }

  virtual bool isConnectionAccepted(ConnectionItem* connection) const override {
    return PortItem<Dir>::isConnectionAccepted(connection) and dynamic_cast<Connection*>(connection);
  }

  void preGeometryChange() override {
    PortItem<Dir>::preGeometryChange();
    connections_.preGeometryChange();
  }

  QJsonObject toJson() const override { return {{"parent", PortItem<Dir>::toJson()}, {"data", connections_.toJson()}}; }

  TypedPortItem(QGraphicsItem* parent = nullptr) : TypedPortItem{Data{}, parent} {}
  TypedPortItem(QJsonObject json, QGraphicsItem* parent = nullptr)
      : TypedPortItem{{json.value("data").toObject()}, json.value("parent").toObject(), parent} {}
};
