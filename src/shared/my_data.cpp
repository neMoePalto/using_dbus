#include "my_data.h"


my_data::my_data(double a, double b, int c)
  : a_(a)
  , b_(b)
  , c_(c) {
}


double my_data::do_something() {
  return a_ + b_ + c_;;
}


QDBusArgument& operator<<(QDBusArgument& arg, const my_data& d) {
  arg.beginStructure();
  arg << d.a_;
  arg << d.b_;
  arg << d.c_;
  arg.endStructure();

  return arg;
}


const QDBusArgument& operator>>(const QDBusArgument& arg, my_data& d) {
  arg.beginStructure();
  arg >> d.a_;
  arg >> d.b_;
  arg >> d.c_;
  arg.endStructure();

  return arg;
}
