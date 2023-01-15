#pragma once

#include <QSettings>
#include <QString>
#include <QWidget>

#include <memory>


class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class connector_widget : public QWidget {
  Q_OBJECT

public:
  connector_widget(QWidget* parent = nullptr);
  ~connector_widget();

private:
  void load_settings(QLineEdit& ip_le, QLineEdit& port_le);

public:
  QComboBox*   dbus_type_cb_;
  QPushButton* connect_pb_;
  const std::pair<QString, QString> connect_pb_names_;
  QLabel*      dbus_registration_label_;
  QLabel*      dbus_registration_label_value_;
  QWidget*     settings_w_;
  QLineEdit*   ip_le_;
  QLineEdit*   port_le_;

  std::unique_ptr<QSettings> settings_;
};
