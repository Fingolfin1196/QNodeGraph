#pragma once
#include <QtCore/QString>
#include <QtCore/QHash>

struct QStringHash {
  std::size_t operator()(const QString& string) const { return qHash(string); }
};
