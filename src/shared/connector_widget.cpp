#include "connector_widget.h"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


connector_widget::connector_widget(QWidget *parent)
  : QWidget(parent)
  , connect_pb_names_{tr("Подключиться"), tr("Отсоединиться")} {

  auto* dbus_type_label = new QLabel(tr("Тип шины:"));
  dbus_type_cb_ = new QComboBox();
  dbus_type_cb_->addItem(tr("Local (Session)"));
  dbus_type_cb_->addItem(tr("Remote (user launched)"));
  dbus_type_cb_->setFixedWidth(250);

  reg_service_label_ = new QLabel();
  reg_service_label_->setMinimumHeight(20);
  reg_service_label_value_ = new QLabel();
  reg_service_label_value_->setMinimumHeight(20);

  connect_pb_ = new QPushButton(this);
  connect_pb_->setCheckable(true);
  connect_pb_->setFixedSize(200, 80);
  //------------------------------------------------

  auto* ip_label   = new QLabel(tr("IP-адрес:"));
  auto* port_label = new QLabel(tr("Tcp-порт:"));
  ip_le_ = new QLineEdit();
  ip_le_->setFixedWidth(140);
  port_le_ = new QLineEdit();
  port_le_->setFixedWidth(140);
  load_settings(*ip_le_, *port_le_);

  settings_w_ = new QWidget();
  auto* settings_grid = new QGridLayout(settings_w_);
  settings_w_->setLayout(settings_grid);
  settings_grid->addWidget(ip_label,     0, 0,   1, 1, Qt::AlignRight);
  settings_grid->addWidget(ip_le_,       0, 1,   1, 1);
  settings_grid->addWidget(port_label,   1, 0,   1, 1, Qt::AlignRight);
  settings_grid->addWidget(port_le_,     1, 1,   1, 1);
  //------------------------------------------------

  auto* reg_service_lyt = new QVBoxLayout();
  reg_service_lyt->insertSpacing(0, 15);
  reg_service_lyt->addWidget(reg_service_label_,       Qt::AlignLeft);
  reg_service_lyt->addWidget(reg_service_label_value_, Qt::AlignLeft);
  reg_service_lyt->insertSpacing(3, 15);

  auto* grid = new QGridLayout(this);
  grid->addWidget(dbus_type_label,     0, 0,   1, 2, Qt::AlignLeft | Qt::AlignBottom);
  grid->addWidget(dbus_type_cb_,       1, 0,   1, 2, Qt::AlignLeft);
  grid->addWidget(settings_w_,         2, 0,   2, 2, Qt::AlignLeft);
  grid->addLayout(reg_service_lyt,     2, 3,   2, 2, Qt::AlignRight);
  grid->addWidget(connect_pb_,         0, 3,   2, 2, Qt::AlignRight);
}


connector_widget::~connector_widget() {
  settings_->setValue("IP",   ip_le_->text());
  settings_->setValue("port", port_le_->text());
}


void connector_widget::load_settings(QLineEdit& ip_le, QLineEdit& port_le) {
  settings_ = std::make_unique<QSettings>("../config/adaptor.ini", QSettings::IniFormat);

  ip_le.setText(settings_->value("IP").toString());
  port_le.setText(settings_->value("port").toString());
}
