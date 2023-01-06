#include <QApplication>

#include "widget_with_adaptor.h"
#include "../shared/my_data.h"


int main(int argc, char* argv[]) {
  register_meta_type<my_data>();

  QApplication a(argc, argv);
  widget_with_adaptor w;
  w.show();

  return a.exec();
}
