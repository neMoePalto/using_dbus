#include "widget_with_adaptor.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "summa_adaptor.h"


widget_with_adaptor::widget_with_adaptor(QWidget *parent)
  : QWidget(parent)
  , connect_pb_names_{tr("Подключиться"), tr("Отсоединиться")} {
  QFont mono_12_db   ("Monospace", 12, QFont::DemiBold);
  QFont mono_14_db   ("Monospace", 14, QFont::DemiBold);
  QFont courier_12_db("Courier",   12, QFont::DemiBold);
  QFont arial_12     ("Arial",     12, QFont::Normal);
  setFont(arial_12);

  auto* header_label = new QLabel(tr("Адаптор DBus (как бы сервер)"));
  header_label->setFont(mono_14_db);
  //------------------------------------------------

  auto* dbus_type_label = new QLabel(tr("Тип шины:"));
  dbus_type_cb_ = new QComboBox();
  dbus_type_cb_->addItem(tr("Local (Session)"));
  dbus_type_cb_->addItem(tr("Remote (user launched)"));
  dbus_type_cb_->setFixedWidth(250);

  auto* reg_service_label = new QLabel(tr("Зарегистрирован сервис:"));
  reg_service_label->setMinimumHeight(20);
  reg_service_label_value_ = new QLabel();
  reg_service_label_value_->setMinimumHeight(20);

  connect_pb_ = new QPushButton(this);
  connect_pb_->setCheckable(true);
  connect_pb_->setFixedSize(200, 80);
  QColor orange_color(210, 120, 34);
  connect_pb_->setPalette(orange_color);
  //------------------------------------------------

  auto* ip_label   = new QLabel(tr("IP-адрес:"));
  auto* port_label = new QLabel(tr("Tcp-порт:"));
  ip_le_ = new QLineEdit();
  ip_le_->setFixedWidth(140);
  port_le_ = new QLineEdit();
  port_le_->setFixedWidth(140);
  load_settings(*ip_le_, *port_le_);

  auto* settings_w = new QWidget();
  auto* settings_grid = new QGridLayout(settings_w);
  settings_w->setLayout(settings_grid);
  settings_grid->addWidget(ip_label,     0, 0,   1, 1, Qt::AlignRight);
  settings_grid->addWidget(ip_le_,       0, 1,   1, 1);
  settings_grid->addWidget(port_label,   1, 0,   1, 1, Qt::AlignRight);
  settings_grid->addWidget(port_le_,     1, 1,   1, 1);
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

  auto* reg_service_lyt = new QVBoxLayout();
  reg_service_lyt->insertSpacing(0, 15);
  reg_service_lyt->addWidget(reg_service_label,        Qt::AlignLeft);
  reg_service_lyt->addWidget(reg_service_label_value_, Qt::AlignLeft);
  reg_service_lyt->insertSpacing(3, 15);

  auto* grid = new QGridLayout(this);
  grid->addWidget(header_label,        0, 0,   2, 5, Qt::AlignHCenter | Qt::AlignTop);
  grid->addWidget(dbus_type_label,     2, 0,   1, 2, Qt::AlignLeft);
  grid->addWidget(dbus_type_cb_,       3, 0,   1, 2, Qt::AlignLeft);
  grid->addWidget(settings_w,          4, 0,   2, 2, Qt::AlignLeft);
  grid->addLayout(reg_service_lyt,     4, 3,   2, 2, Qt::AlignRight);
  grid->addWidget(connect_pb_,         2, 3,   2, 2, Qt::AlignRight);
  grid->addWidget(output_label,        6, 0,   1, 5, Qt::AlignHCenter | Qt::AlignBottom);
  grid->addWidget(output_te_,          7, 0,  18, 5);
  grid->addWidget(send_by_signal_pb,  26, 0,   2, 2);
  setLayout(grid);

  connect(send_by_signal_pb, &QPushButton::clicked, [this]() {
    static int ch = 0;
    ch++;
    bool_data_signal(ch % 2 == 0);
  });

  connect(dbus_type_cb_, &QComboBox::currentIndexChanged, [settings_w, this](int index) {
    index == 0 ? settings_w->hide() : settings_w->show();
    disconnect_from_dbus();
  });
  emit dbus_type_cb_->currentIndexChanged(0);

  // Связываем класс-клиент с Адаптором:
  auto* ptr = new SumAdaptor(this);
  Q_UNUSED(ptr);
}


widget_with_adaptor::~widget_with_adaptor() {
  settings_->setValue("IP",   ip_le_->text());
  settings_->setValue("port", port_le_->text());
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
  if (dbus_type_cb_->currentIndex() == 0) {
    if (!session_conn_.isConnected()) {
      QString s = "Connection with Session DBus... ";
      session_conn_ = QDBusConnection::sessionBus();
      output_te_->append(s + (session_conn_.isConnected() ? "OK" : "FAILED !"));
    }
    current_conn_ = &session_conn_;
  } else {
    const QString current_settings = "tcp:host=" + ip_le_->text() + ",port=" + port_le_->text();
    if (!remote_conn_.isConnected() || prev_remote_conn_settings_ != current_settings) {
      prev_remote_conn_settings_ = current_settings;
      QString s = "Connection with user-launched DBus on tcp... ";
      remote_conn_ = QDBusConnection::connectToBus(current_settings, "abc" + port_le_->text());
      output_te_->append(s + (remote_conn_.isConnected() ? "OK" : "FAILED !"));
    }
    current_conn_ = &remote_conn_;
  }

  if (current_conn_->isConnected()) {
    // 1. Регистрируем имя сервиса.
    // Имя сервиса должно совпадать с именем интерфейса, который описан в xml-файле
    if (current_conn_->registerService(service_name_)) {
      output_te_->append("  Service registered");
      reg_service_label_value_->setText(service_name_);
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

    connect_pb_->setText(connect_pb_names_.second);
    disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
    connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
  } else {
    connect_pb_->setChecked(false);
  }
}


void widget_with_adaptor::disconnect_from_dbus() {
  if (connect_pb_->text() != connect_pb_names_.first) {
    connect_pb_->setText(connect_pb_names_.first);
    disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
    connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
  }

  if (connect_pb_->isChecked()) {
    connect_pb_->setChecked(false);
  }

  if (current_conn_ && current_conn_->isConnected()) {
    current_conn_->unregisterObject(object_path_);
//    QDBusConnection::disconnectFromBus(connect_name_); - no effect
    if (current_conn_->unregisterService(service_name_)) {
      output_te_->append("  Disconnected (service unregistered)");
      reg_service_label_value_->clear();
    } else {
//      output_te_->append("Error! Can't unregister service.");
    }
  }
}


void widget_with_adaptor::load_settings(QLineEdit& ip_le, QLineEdit& port_le) {
  settings_ = std::make_unique<QSettings>("../config/adaptor.ini", QSettings::IniFormat);

  ip_le.setText(settings_->value("IP").toString());
  port_le.setText(settings_->value("port").toString());
}
