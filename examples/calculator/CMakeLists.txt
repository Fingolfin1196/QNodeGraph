add_executable(QCalculatorGraph)

set_target_properties(QCalculatorGraph PROPERTIES CXX_STANDARD 20)
target_compile_options(QCalculatorGraph PRIVATE $<$<CONFIG:RELEASE>:-O3;-march=native>)
target_compile_options(QCalculatorGraph PRIVATE $<$<CONFIG:DEBUG>:-O0;-g>)
# Some parameters are unused in release mode but used in debug mode, which clang does not respect!
target_compile_options(QCalculatorGraph PRIVATE -Wpedantic -Werror -Wextra -Wno-error=unused-parameter)

# TypedLineEdit.hpp as a template is not supported!
qt5_wrap_cpp(MOC_SOURCES NumberGraphView.hpp TARGET QCalculatorGraph)
target_sources(QCalculatorGraph PRIVATE ${MOC_SOURCES})

target_link_libraries(QCalculatorGraph QNodeGraph)

file(GLOB_RECURSE SOURCES ./*.cpp)
target_sources(QCalculatorGraph PRIVATE ${SOURCES})
