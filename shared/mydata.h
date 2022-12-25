#ifndef MYDATA_H
#define MYDATA_H
#include <QtDBus>

struct MyData
{
    MyData() = default;
    MyData(double a1, double a2, int a3);
    double makeSum();
    // Реализуем необходимые операторы (2 шт):
    friend QDBusArgument& operator<<(QDBusArgument& bus, const MyData& d);
    friend const QDBusArgument& operator>>(const QDBusArgument& bus, MyData& d);
    // Регистрируем наш тип данных в Qt и QDBus. Методы просто объединяет два вызова,
    // которые можно сделать явно, в начале функции main():
    static void registerMetaType();
private:
    double _a{};
    double _b{};
    int _c{};
};

// "Знакомим" систему метатипов Qt с нашим типом данных:
Q_DECLARE_METATYPE(MyData)

#endif // MYDATA_H
