project(interface_for_dbus VERSION 0.1 LANGUAGES CXX)

set(sources
  main.cpp
  widgetwithinterface.cpp
  widgetwithinterface.h
  ../shared/mydata.cpp
  ../shared/mydata.h
)

set(xml
  ../shared/summa.xml
)

set_source_files_properties(${xml} PROPERTIES
  INCLUDE ../shared/mydata.h
)

#set(dbus_interface_src)

qt_add_dbus_interface(dbus_interface_src
  ${xml}
  summa_interface
)


qt_add_executable(interface_for_dbus
  ${sources}
  ${dbus_interface_src}
)

target_link_libraries(interface_for_dbus PRIVATE
  Qt::Core
  Qt::DBus
  Qt::Gui
  Qt::Widgets
)
