project(adaptor_for_dbus VERSION 0.1 LANGUAGES CXX)

set(sources
  main.cpp
  widgetwithadaptor.cpp
  widgetwithadaptor.h
  ../shared/mydata.cpp
  ../shared/mydata.h
)

#set(dbus_adaptor_src)

qt_add_dbus_adaptor(dbus_adaptor_src
  ../shared/summa.xml
  ../shared/mydata.h
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
