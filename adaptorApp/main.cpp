#include "widgetwithadaptor.h"
#include "../shared/mydata.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    MyData::registerMetaType();
    QApplication a(argc, argv);
    WidgetWithAdaptor w;
    w.show();

    return a.exec();
}
