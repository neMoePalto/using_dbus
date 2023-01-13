#pragma once

#include <QDBusConnection>
#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>

#include "../shared/my_data.h"


class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

// Класс-клиент, использующий адаптор DBus (Адаптор)
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
  QComboBox*       dbus_type_cb_;
  QPushButton*     connect_pb_;
  const std::pair<QString, QString> connect_pb_names_;
  QLabel*          reg_service_label_value_;
  QLineEdit*       ip_le_;
  QLineEdit*       port_le_;
  QTextEdit*       output_te_;

//  QString    connect_name_;//{"sample_connect_name"}; // TODO: Проверить необходимость для remote-шины
  const QString    service_name_{"org.rumba.Sum"};
  const QString    object_path_{"/"};
  QDBusConnection* current_conn_ = nullptr;
  QDBusConnection  session_conn_{""};
  QDBusConnection  remote_conn_{""};
  QString          prev_remote_conn_settings_;

  std::unique_ptr<QSettings> settings_;
};
