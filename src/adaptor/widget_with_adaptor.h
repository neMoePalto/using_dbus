#pragma once

#include <QDBusConnection>
#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>

#include "../shared/my_data.h"


class QTextEdit;
class connector_widget;

// Класс-клиент, использующий адаптор DBus (Адаптор)
class widget_with_adaptor : public QWidget {
  Q_OBJECT

public:
  widget_with_adaptor(QWidget* parent = nullptr);

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

private:
  connector_widget* w_;
  QTextEdit*        output_te_;

//  QString    connect_name_;//{"sample_connect_name"}; // TODO: Проверить необходимость для remote-шины
  const QString     service_name_{"org.rumba.Sum"};
  const QString     object_path_{"/"};
  QDBusConnection*  current_conn_ = nullptr;
  QDBusConnection   session_conn_{""};
  QDBusConnection   remote_conn_{""};
  QString           prev_remote_conn_settings_;
};
