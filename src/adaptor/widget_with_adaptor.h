#pragma once

#include <QDBusConnection>
#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>

#include "../shared/my_data.h"


class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

// Класс-клиент, использующий адаптор Dbus (Адаптор)
class widget_with_adaptor : public QWidget {
  Q_OBJECT

public:
  widget_with_adaptor(QWidget *parent = nullptr);
  ~widget_with_adaptor();

public slots:
  // В отличие от Интерфейса, генерирующего свои слоты по содержимому xml-файла,
  // в классе-клиенте, работающем через Адаптор, их нужно прописывать руками:
  int get_sum_method(int a, int b);
  void void_method(my_data obj, double a);

signals:
  void bool_data_signal(bool);

// Слоты, не связанные с Адаптором:
private:
  void connect_to_dbus();
  void disconnect_from_dbus();
  void load_settings(QLineEdit& ip_le, QLineEdit& port_le);

private:
  QLabel*         reg_service_label_value_;
  QLineEdit*      ip_le_;
  QLineEdit*      port_le_;
  QPushButton*    connect_pb_;
  QTextEdit*      output_te_;

  const QString   connect_name_{"sample_connect_name"};
  const QString   service_name_{"org.rumba.Sum"};
  const QString   object_path_{"/"};
  QDBusConnection connection_{""};

  std::unique_ptr<QSettings> settings_;
};
