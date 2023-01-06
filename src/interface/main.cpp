#include <QApplication>

#include "widget_with_interface.h"
#include "../shared/my_data.h"


int main(int argc, char *argv[]) {
  register_meta_type<my_data>();

  QApplication a(argc, argv);
  widget_with_interface w;
  w.show();

  return a.exec();
}
