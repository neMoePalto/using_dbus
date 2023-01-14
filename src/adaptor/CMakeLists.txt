project(adaptor_for_dbus VERSION 0.1 LANGUAGES CXX)

set(sources
  main.cpp
  widget_with_adaptor.cpp
  widget_with_adaptor.h
  ../shared/connector_widget.cpp
  ../shared/connector_widget.h
  ../shared/my_data.cpp
  ../shared/my_data.h
)

#set(dbus_adaptor_src)

qt_add_dbus_adaptor(dbus_adaptor_src
  ../shared/summa.xml
  ../shared/my_data.h
  ""
  summa_adaptor
)


qt_add_executable(adaptor_for_dbus
  ${sources}
  ${dbus_adaptor_src}
)

target_link_libraries(adaptor_for_dbus PRIVATE
  Qt::Core
  Qt::DBus
  Qt::Gui
  Qt::Widgets
)
