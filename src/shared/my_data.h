#pragma once

#include <boost/type_index.hpp>

#include <QtDBus>


struct my_data {
  my_data() = default;
  my_data(double a, double b, int c);
  double do_something();
  // Реализуем необходимые операторы (2 шт):
  friend QDBusArgument& operator<<(QDBusArgument& bus, const my_data& d);
  friend const QDBusArgument& operator>>(const QDBusArgument& bus, my_data& d);

private:
  double a_;
  double b_;
  int    c_;
};

// "Знакомим" систему метатипов Qt с нашим типом данных:
Q_DECLARE_METATYPE(my_data)


// Регистрируем наш тип данных в Qt и QDBus. Методы просто объединяет два вызова,
// которые можно сделать явно, в начале функции main():
template <typename T>
void register_meta_type() {
  qRegisterMetaType<T>(boost::typeindex::type_id<T>().pretty_name().c_str());
  qDBusRegisterMetaType<T>();
}
