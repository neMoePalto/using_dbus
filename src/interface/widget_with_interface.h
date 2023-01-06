#pragma once

#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>

#include "summa_interface.h"


class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class widget_with_interface : public QWidget {
  Q_OBJECT

public:
  widget_with_interface(QWidget *parent = nullptr);
  ~widget_with_interface();

private:
  void connect_to_dbus();
  void disconnect_from_dbus();
  void load_settings(QLineEdit& ip_le, QLineEdit& port_le);

private:
  // Интерфейс DBus, используется для отправки сообщений:
  std::unique_ptr<org::rumba::Sum> sum_iface_;

  QLabel*       id_on_dbus_label_value_;
  QLineEdit*    ip_le_;
  QLineEdit*    port_le_;
  QPushButton*  connect_pb_;
  QTextEdit*    output_te_;

  const QString connect_name_{"sample_connect_name"};
  const QString object_path_{"/"};

  std::unique_ptr<QSettings> settings_;
};
