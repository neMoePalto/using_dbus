#include "widget_with_adaptor.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "../shared/connector_widget.h"
#include "summa_adaptor.h"


widget_with_adaptor::widget_with_adaptor(QWidget *parent)
  : QWidget(parent) {
  QFont mono_12_db   ("Monospace", 12, QFont::DemiBold);
  QFont mono_14_db   ("Monospace", 14, QFont::DemiBold);
  QFont courier_12_db("Courier",   12, QFont::DemiBold);
  QFont arial_12     ("Arial",     12, QFont::Normal);
  setFont(arial_12);

  auto* header_label = new QLabel(tr("Адаптор DBus (как бы сервер)"));
  header_label->setFont(mono_14_db);
  //------------------------------------------------

  w_ = new connector_widget(this);
  QColor orange_color(210, 120, 34);
  w_->connect_pb_->setPalette(orange_color);
  w_->reg_service_label_->setText("Зарегистрирован сервис:");
  //------------------------------------------------

  auto* output_label = new QLabel(tr("Логи подключения и данные, полученные от интерфейсов"));
  output_label->setFixedHeight(35);
  output_te_ = new QTextEdit();
  output_te_->setFont(courier_12_db);
  output_te_->setMinimumSize(220, 350);

  auto* send_by_signal_pb = new QPushButton(
                               tr("Отправить данные в адрес всех\nподключенных"
                                  " к сервису клиентов,\nиспользуя сигнал <signal>"), this);
  send_by_signal_pb->setFixedHeight(80);
  send_by_signal_pb->setPalette(orange_color);
  //------------------------------------------------

  auto* grid = new QGridLayout(this);
  grid->addWidget(header_label,        0, 0,   2, 5, Qt::AlignHCenter | Qt::AlignTop);
  grid->addWidget(w_,                  2, 0,   4, 5);
  grid->addWidget(output_label,        6, 0,   1, 5, Qt::AlignHCenter | Qt::AlignBottom);
  grid->addWidget(output_te_,          7, 0,  18, 5);
  grid->addWidget(send_by_signal_pb,  26, 0,   2, 2);
  setLayout(grid);

  connect(send_by_signal_pb, &QPushButton::clicked, [this]() {
    static int ch = 0;
    ch++;
    bool_data_signal(ch % 2 == 0);
  });

  connect(w_->dbus_type_cb_, &QComboBox::currentIndexChanged, [this](int index) {
    index == 0 ? w_->settings_w_->hide() : w_->settings_w_->show();
    disconnect_from_dbus();
  });
  emit w_->dbus_type_cb_->currentIndexChanged(0);

  // Связываем класс-клиент с Адаптором:
  auto* ptr = new SumAdaptor(this);
  Q_UNUSED(ptr);
}


int widget_with_adaptor::get_sum_method(int a, int b) {
  int sum = a + b;
  output_te_->append(QString::number(a) + " + " +
                     QString::number(b) + " = " + QString::number(sum));
//  bool_data_signal(false); - можно, например, отправить еще кому-то оповещение через сигнал
  return sum;
}


void widget_with_adaptor::void_method(my_data obj, double a) {
  output_te_->append(QObject::tr("void-метод. Сумма значений членов класса = ") +
                     QString::number(obj.do_something()));
}


void widget_with_adaptor::connect_to_dbus() {
  if (w_->dbus_type_cb_->currentIndex() == 0) {
    if (!session_conn_.isConnected()) {
      QString s = "Connection with Session DBus... ";
      session_conn_ = QDBusConnection::sessionBus();
      output_te_->append(s + (session_conn_.isConnected() ? "OK" : "FAILED !"));
    }
    current_conn_ = &session_conn_;
  } else {
    const QString current_settings = "tcp:host=" + w_->ip_le_->text() + ",port=" + w_->port_le_->text();
    if (!remote_conn_.isConnected() || prev_remote_conn_settings_ != current_settings) {
      prev_remote_conn_settings_ = current_settings;
      QString s = "Connection with user-launched DBus on tcp... ";
      remote_conn_ = QDBusConnection::connectToBus(current_settings, "abc" + w_->port_le_->text());
      output_te_->append(s + (remote_conn_.isConnected() ? "OK" : "FAILED !"));
    }
    current_conn_ = &remote_conn_;
  }

  if (current_conn_->isConnected()) {
    // 1. Регистрируем имя сервиса.
    // Имя сервиса должно совпадать с именем интерфейса, который описан в xml-файле
    if (current_conn_->registerService(service_name_)) {
      output_te_->append("  Service registered");
      w_->reg_service_label_value_->setText(service_name_);
    } else {
      output_te_->append("  Error: Can't register service " + service_name_ +
                         ".\nMay be, this name was already registered.");
      return;
    }
    // 2. Регистрируем объект (часто имя объекта - последняя часть имени сервиса).
    // Не совсем ясно, почему это необходимо делать явно
    if (!current_conn_->registerObject(object_path_, this)) {
      output_te_->append("  Error: Can't register object: " + object_path_);
      return;
    }

    w_->connect_pb_->setText(w_->connect_pb_names_.second);
    disconnect(w_->connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
    connect   (w_->connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
  } else {
    w_->connect_pb_->setChecked(false);
  }
}


void widget_with_adaptor::disconnect_from_dbus() {
  if (w_->connect_pb_->text() != w_->connect_pb_names_.first) {
    w_->connect_pb_->setText(w_->connect_pb_names_.first);
    disconnect(w_->connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
    connect   (w_->connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
  }

  if (w_->connect_pb_->isChecked()) {
    w_->connect_pb_->setChecked(false);
  }

  if (current_conn_ && current_conn_->isConnected()) {
    current_conn_->unregisterObject(object_path_);
//    QDBusConnection::disconnectFromBus(connect_name_); - no effect
    if (current_conn_->unregisterService(service_name_)) {
      output_te_->append("  Disconnected (service unregistered)");
      w_->reg_service_label_value_->clear();
    } else {
//      output_te_->append("Error! Can't unregister service.");
    }
  }
}
