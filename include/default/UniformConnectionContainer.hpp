#pragma once
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

template<typename Connection> class UniformConnectionContainer : public std::vector<Connection*> {
  using connections_t = std::vector<Connection*>;

public:
  void insert(Connection* connection) { this->push_back(connection); }

  void remove(Connection* connection) {
    auto it{std::find(this->begin(), this->end(), connection)};
    assert(it != this->end());
    this->erase(it);
  }

  void preGeometryChange() {
    for (auto connection : *this) connection->preGeometryChange();
  }

  QJsonObject toJson() const {
    QJsonArray connections{};
    for (auto* connection : *this) connections.append(QJsonObject{{"address", ptrToStr(connection)}});
    return {{"connections", connections}};
  }

  UniformConnectionContainer() = default;
  /**
   * This requires the connections to be created outside of the container, which is not ideal but the best way I see.
   */
  UniformConnectionContainer(QJsonObject) : UniformConnectionContainer{} {}
};