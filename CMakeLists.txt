cmake_minimum_required(VERSION 3.5)

project(dbus_communication LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS
  DBus
  Widgets
)

add_subdirectory(adaptorApp)

add_subdirectory(interfaceApp)
