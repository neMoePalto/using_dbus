project(interface VERSION 0.1 LANGUAGES CXX)

set(sources
  main.cpp
  widgetwithinterface.cpp
  widgetwithinterface.h
  ../shared/mydata.cpp
  ../shared/mydata.h
)

set(xml ../shared/summa.xml)

set_source_files_properties(${xml} PROPERTIES
  INCLUDE ../shared/mydata.h
)

set(dbus_interfaces)

qt_add_dbus_interface(dbus_interfaces
  ${xml}
  summa_interface
)

add_executable(interface
  ${sources}
  ${dbus_interfaces}
)

target_link_libraries(interface PRIVATE
  Qt::DBus
  Qt::Widgets
)
