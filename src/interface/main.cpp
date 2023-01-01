#include <QApplication>

#include "widget_with_interface.h"
#include "../shared/my_data.h"


int main(int argc, char *argv[])
{
    MyData::registerMetaType();
//    qRegisterMetaType<MyData>("MyData");
//    qDBusRegisterMetaType<MyData>();

    QApplication a(argc, argv);
    WidgetWithInterface w;
    w.show();

    return a.exec();
}
