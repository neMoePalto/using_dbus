#include "my_data.h"


MyData::MyData(double a1, double a2, int a3)
{
    _a = a1;
    _b = a2;
    _c = a3;
}

double MyData::makeSum()
{
    double res;
    res = _a + _b + _c;
    return res;
}

QDBusArgument& operator<<(QDBusArgument& arg, const MyData& d)
{
    arg.beginStructure();
    arg << d._a;
    arg << d._b;
    arg << d._c;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, MyData& d)
{
    arg.beginStructure();
    arg >> d._a;
    arg >> d._b;
    arg >> d._c;
    arg.endStructure();

    return arg;
}

void MyData::registerMetaType()
{
    qRegisterMetaType<MyData>("MyData");
    qDBusRegisterMetaType<MyData>();
}
