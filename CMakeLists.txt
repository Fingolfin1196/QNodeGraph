cmake_minimum_required(VERSION 3.16)

project(QNodeGraph)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
find_package(KF5ConfigWidgets REQUIRED)

option(BUILD_EXAMPLES "Build Examples" ON)

add_library(QNodeGraph SHARED)

set_target_properties(QNodeGraph PROPERTIES CXX_STANDARD 20)
target_compile_options(QNodeGraph PRIVATE $<$<CONFIG:RELEASE>:-Ofast;-march=native>)
target_compile_options(QNodeGraph PRIVATE $<$<CONFIG:DEBUG>:-O0;-g>)
target_compile_options(QNodeGraph PRIVATE -Wpedantic -Werror -Wextra -Wno-error=unused-parameter)

set(SOURCES src/ConnectionItem.cpp src/NodeGraphScene.cpp src/NodeGraphView.cpp src/NodeItemGroup.cpp src/NodeProxy.cpp
            src/PortItem.cpp)
target_link_libraries(QNodeGraph PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui)
qt5_wrap_cpp(
  SOURCES
  include/ConnectionItem.hpp
  include/NodeGraphScene.hpp
  include/NodeGraphView.hpp
  include/NodeProxy.hpp
  TARGET
  QNodeGraph)
target_sources(QNodeGraph PRIVATE ${SOURCES})

target_link_libraries(QNodeGraph PUBLIC KF5::ConfigWidgets)

target_include_directories(QNodeGraph PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_sources(QNodeGraph PRIVATE src/ConnectionItem.cpp src/NodeGraphScene.cpp src/NodeGraphView.cpp
                                  src/NodeItemGroup.cpp src/NodeProxy.cpp src/PortItem.cpp)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

include(GNUInstallDirs)
install(
  TARGETS QNodeGraph
  EXPORT QNodeGraphTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QNodeGraph)
install(
  EXPORT QNodeGraphTargets
  FILE QNodeGraphTargets.cmake
  NAMESPACE QNodeGraph::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QNodeGraph)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/QNodeGraphConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/QNodeGraphConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QNodeGraph)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QNodeGraphConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QNodeGraph)
