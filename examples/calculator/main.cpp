#include "NodeGraphScene.hpp"
#include "NumberGraphView.hpp"
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <iostream>

int main(int argc, char* argv[]) {
  QApplication app{argc, argv};

  NodeGraphScene scene{};
  auto* view{new NumberGraphView(&scene)};

  auto* layout{new QHBoxLayout};
  layout->addWidget(view);

  auto* container{new QWidget};
  container->setAcceptDrops(true);
  container->setLayout(layout);

  QMainWindow window{};

  auto* save_action{new QAction{"Save"}};
  QObject::connect(save_action, &QAction::triggered, [&window, view](bool) {
    QUrl home{QUrl::fromLocalFile(QDir::home().absolutePath())};
    auto url{QFileDialog::getSaveFileUrl(&window, "Save graph", home, "Graph file (*.graph)")};
    std::cout << "Save: " << url.path().toStdString() << std::endl;
    QFile file{url.path()};
    if (not file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    file.write(view->toJson().toJson());
    file.close();
  });
  auto* load_action{new QAction{"Load"}};
  QObject::connect(load_action, &QAction::triggered, [&window, view](bool) {
    QUrl home{QUrl::fromLocalFile(QDir::home().absolutePath())};
    auto url{QFileDialog::getOpenFileUrl(&window, "Load graph", home, "Graph file (*.graph)")};
    std::cout << "Load: " << url.path().toStdString() << std::endl;
    QFile file{url.path()};
    if (not file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    view->fromJson(QJsonDocument::fromJson(file.readAll()));
    file.close();
  });
  auto* menu{new QMenu{"File"}};
  menu->addAction(load_action);
  menu->addAction(save_action);

  window.setCentralWidget(container);
  window.menuBar()->addMenu(menu);
  window.setAcceptDrops(true);
  window.showMaximized();

  return QApplication::exec();
}
