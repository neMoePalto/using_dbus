#include "widgetwithinterface.h"
#include "../shared/mydata.h"
#include <QApplication>

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
