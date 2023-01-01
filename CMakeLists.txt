cmake_minimum_required(VERSION 3.5)

project(dbus_communication LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)


find_package(Qt6 REQUIRED COMPONENTS
  Core
  DBus
  Gui
  Widgets
)

add_subdirectory(src/adaptor)

add_subdirectory(src/interface)
