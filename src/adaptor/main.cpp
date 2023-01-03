#include <QApplication>

#include "widget_with_adaptor.h"
#include "../shared/my_data.h"


int main(int argc, char* argv[]) {
  MyData::registerMetaType();

  QApplication a(argc, argv);
  widget_with_adaptor w;
  w.show();

  return a.exec();
}
