#include "widget_with_adaptor.h"

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "summa_adaptor.h"


widget_with_adaptor::widget_with_adaptor(QWidget *parent)
  : QWidget(parent) {
  QFont mono_12_db("Monospace",  12, QFont::DemiBold);
  QFont mono_14_db("Monospace",  14, QFont::DemiBold);
  QFont courier_12_db("Courier", 12, QFont::DemiBold);
  QFont arial_12("Arial",        12, QFont::Normal);
  setFont(arial_12);

  auto* settings_label = new QLabel(tr("Настройки для соединения с пользовательской шиной Dbus:"));
  // TODO: Добавить комбобокс Шина: локальная или удаленная (настр. нужны только для нее)
  // "При незаполненных полях устанавливается соединение с локальной\nсессионной шиной."
  auto* ip_label   = new QLabel(tr("IP-адрес:"));
  auto* port_label = new QLabel(tr("Tcp-порт:"));
  auto* serv_reg_label  = new QLabel(tr("Зарегистрирован сервис:"));
  serv_reg_value_label_ = new QLabel();
  ip_le_ = new QLineEdit();
  ip_le_->setFixedWidth(180);
  port_le_ = new QLineEdit();
  port_le_->setFixedWidth(180);
  load_settings(*ip_le_, *port_le_);

  connect_pb_ = new QPushButton(this);
  connect_pb_->setFixedSize(180, 70);
  QColor orange_color(210, 120, 34);
  connect_pb_->setPalette(orange_color);

  output_te_ = new QTextEdit();
  output_te_->setFont(courier_12_db);
  output_te_->setMinimumSize(220, 350);
  auto* header_label = new QLabel(tr("Адаптор Dbus (как бы сервер)"));
  header_label->setFont(mono_14_db);
  auto* output_label = new QLabel(tr("Логи подключения + данные, полученные от interfaceApp"));
  output_label->setFixedHeight(35);

  auto* send_by_signal_pb_ = new QPushButton(
                               tr("Отправить данные в адрес всех\nподключенных"
                                  " к сервису клиентов,\nиспользуя сигнал <signal>"), this);
  send_by_signal_pb_->setFixedHeight(80);
  send_by_signal_pb_->setPalette(orange_color);

  auto* grid = new QGridLayout(this);
  grid->addWidget(header_label,           0, 0,   1, 4, Qt::AlignCenter);
  grid->addWidget(settings_label,         1, 0,   1, 4, Qt::AlignHCenter);
  grid->addWidget(ip_label,               2, 0,   1, 1, Qt::AlignRight);
  grid->addWidget(ip_le_,                 2, 1,   1, 1);
  grid->addWidget(connect_pb_,            2, 2,   2, 2, Qt::AlignRight);
  grid->addWidget(port_label,             3, 0,   1, 1, Qt::AlignRight);
  grid->addWidget(port_le_,               3, 1,   1, 1);
  grid->addWidget(serv_reg_label,         4, 0,   1, 1, Qt::AlignRight | Qt::AlignTop);
  grid->addWidget(serv_reg_value_label_,  4, 1,   1, 1, Qt::AlignTop);

  grid->addWidget(output_label,           5, 0,   1, 4, Qt::AlignHCenter | Qt::AlignBottom);
  grid->addWidget(output_te_,             6, 0,   8, 4);
  grid->addWidget(send_by_signal_pb_,    15, 0,   2, 2);
  setLayout(grid);

  disconnect_from_dbus();
  connect(send_by_signal_pb_, &QPushButton::clicked, [this]() {
    static int ch = 0;
    ch++;
    bool_data_signal(ch % 2 == 0);
  });

  // 1. Связываем класс-клиент с Адаптором:
  auto ptr = new SumAdaptor(this);
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


void widget_with_adaptor::void_method(MyData obj, double a) {
  output_te_->append(QObject::tr("void-метод. Сумма значений членов класса = ") +
                     QString::number(obj.makeSum()));
}


void widget_with_adaptor::connect_to_dbus() {
  const QString path = "tcp:host=" + ip_le_->text() + ",port=" + port_le_->text();
  connection_ = QDBusConnection::sessionBus();
  // Раскомментировать для работы через remote host:
  //  auto connection = QDBusConnection::connectToBus(path, connect_name_);

  if (connection_.isConnected()) {
    output_te_->append("Connection to DBus with " + ip_le_->text() + ":" + port_le_->text());
    // 2. Регистрируем имя сервиса.
    // Имя сервиса должно совпадать с именем интерфейса, который описан в xml-файле
    if (connection_.registerService(service_name_)) {
      output_te_->append("Success connection.");
      serv_reg_value_label_->setText(service_name_);
    } else {
      output_te_->append("Error: Can't register service " + service_name_ +
                         ".\nMay be, this name was already registered.");
      return;
    }
    // 3. Регистрируем объект (часто имя объекта - последняя часть имени сервиса).
    // Пока не совсем ясно, почему это необходимо делать явно
    connection_.registerObject(object_path_, this);

    connect_pb_->setText(tr("Отсоединиться\nот DBus"));
    disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
    connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
  } else {
    output_te_->append("Connection failed!");
    QDBusConnection::disconnectFromBus(connect_name_);
  }
}


void widget_with_adaptor::disconnect_from_dbus() {
  static bool is_first_call{true};

  if (is_first_call) {
    is_first_call = false;
  } else {
    connection_.unregisterObject(object_path_);
    connection_.unregisterService(service_name_);
    // QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
    // connect_name_ может иметь значение при работе через remote host:
    QDBusConnection::disconnectFromBus(connect_name_);
    output_te_->append("Disconnected from DBus.");
    serv_reg_value_label_->clear();
  }

  disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::disconnect_from_dbus);
  connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_adaptor::connect_to_dbus);
  connect_pb_->setText(tr("Подключиться\nк DBus"));
}


void widget_with_adaptor::load_settings(QLineEdit& leForIp, QLineEdit& leForPort) {
  settings_ = std::make_unique<QSettings>("../config/adaptorApp.ini", QSettings::IniFormat);

  leForIp.setText(settings_->value("IP").toString());
  leForPort.setText(settings_->value("port").toString());
}
