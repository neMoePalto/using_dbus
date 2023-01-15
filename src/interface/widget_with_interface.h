#pragma once

#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>

#include "summa_interface.h"


class QTextEdit;
class connector_widget;

class widget_with_interface : public QWidget {
  Q_OBJECT

public:
  widget_with_interface(QWidget *parent = nullptr);

private:
  void connect_to_dbus();
  void disconnect_from_dbus();

private:
  // Интерфейс DBus, используется для отправки сообщений:
  std::unique_ptr<org::rumba::Sum> sum_iface_;

  connector_widget* w_;
  QTextEdit*        output_te_;

  const QString     connect_name_{"sample_connect_name"};
  const QString     object_path_{"/"};
  QDBusConnection*  current_conn_ = nullptr;
  QDBusConnection   session_conn_{""};
  QDBusConnection   remote_conn_{""};
  QString           prev_remote_conn_settings_;
};
