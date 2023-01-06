#include "widget_with_interface.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "../shared/my_data.h"


widget_with_interface::widget_with_interface(QWidget *parent)
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
  auto* id_on_dbus_label = new QLabel(tr("ID клиента на шине DBus:"));
  id_on_dbus_label_value_ = new QLabel();
  ip_le_ = new QLineEdit();
  ip_le_->setFixedWidth(180);
  port_le_ = new QLineEdit();
  port_le_->setFixedWidth(180);
  load_settings(*ip_le_, *port_le_);

  connect_pb_ = new QPushButton();
  connect_pb_->setFixedSize(180, 70);
  QColor green_color{110, 140, 60};
  connect_pb_->setPalette(green_color);

  output_te_ = new QTextEdit();
  output_te_->setFont(courier_12_db);
  output_te_->setMinimumSize(220, 350);
  auto* header_label = new QLabel(tr("Интерфейс Dbus (как бы клиент)"));
  header_label->setFont(mono_14_db);
  auto* output_label = new QLabel(tr("Логи подключения + данные, полученные от adaptor"));
  output_label->setFixedHeight(35);

  auto* send_by_method_pb = new QPushButton(tr("Отправить данные,\nиспользуя метод <method>"));
  send_by_method_pb->setFixedHeight(80);
  send_by_method_pb->setPalette(green_color);

  auto* grid = new QGridLayout(this);
  grid->addWidget(header_label,             0, 0,   1, 4, Qt::AlignCenter);
  grid->addWidget(settings_label,           1, 0,   1, 4, Qt::AlignHCenter);
  grid->addWidget(ip_label,                 2, 0,   1, 1, Qt::AlignRight);
  grid->addWidget(ip_le_,                   2, 1,   1, 1);
  grid->addWidget(connect_pb_,              2, 2,   2, 2, Qt::AlignRight);
  grid->addWidget(port_label,               3, 0,   1, 1, Qt::AlignRight);
  grid->addWidget(port_le_,                 3, 1,   1, 1);
  grid->addWidget(id_on_dbus_label,         4, 0,   1, 1, Qt::AlignRight | Qt::AlignTop);
  grid->addWidget(id_on_dbus_label_value_,  4, 1,   1, 1, Qt::AlignTop);

  grid->addWidget(header_label,             5, 0,   1, 4, Qt::AlignHCenter | Qt::AlignBottom);
  grid->addWidget(output_te_,               6, 0,   8, 4);
  grid->addWidget(send_by_method_pb,       15, 2,   2, 2);
  setLayout(grid);

  disconnect_from_dbus();
  connect(send_by_method_pb, &QPushButton::clicked, [this]() {
    static int b = 0;

    if (sum_iface_ == nullptr) {
      return;
    }

    b++;
    if (b % 2 == 0) {
      my_data data(2.01, 0.13, 1);
      sum_iface_->void_method(my_data{2.01, 0.13, 1}, 123.045);
    } else {
      QDBusPendingReply<int> reply = sum_iface_->get_sum_method(10, 12);
      reply.waitForFinished();
      QString text = reply.isError() ? reply.error().message()
                                     : "Summa = " + QString::number(reply.value());
      output_te_->append(text);
    }
  });
}


widget_with_interface::~widget_with_interface() {
  settings_->setValue("IP", ip_le_->text());
  settings_->setValue("port", port_le_->text());
}


void widget_with_interface::connect_to_dbus() {
  QString path = "tcp:host=" + ip_le_->text() + ",port=" + port_le_->text();
  qDebug() << path;

  auto connection = QDBusConnection::sessionBus();
  // Раскомментировать для работы через remote host:
  //    auto connection = QDBusConnection::connectToBus(path, _connName);

  if (connection.isConnected()) {
    output_te_->append("Connection to DBus with " + ip_le_->text() + ":" + port_le_->text());
    output_te_->append("Success connection.");
    // При конструировании Интерфейса необходимо задать переменную path.
    // Значение переменной path должно совпадать со значением, указанным
    // на стороне Адаптора.
    sum_iface_ = std::make_unique<org::rumba::Sum>(org::rumba::Sum::staticInterfaceName(),
                                                   object_path_, // "/Sum"
                                                   connection,
                                                   this);
    id_on_dbus_label_value_->setText("\"" + sum_iface_->connection().baseService() + "\"");
    connect(sum_iface_.get(), &OrgRumbaSumInterface::bool_data_signal, [this](bool val) {
      output_te_->append(val ? "true" : "false");
    });

    connect_pb_->setText(tr("Отсоединиться\nот DBus"));
    disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_interface::connect_to_dbus);
    connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_interface::disconnect_from_dbus);
  } else {
    output_te_->append("Connection failed!");
    QDBusConnection::disconnectFromBus(connect_name_);
  }
}


void widget_with_interface::disconnect_from_dbus() {
  static bool is_first_call{true};

  if (is_first_call == true) {
    is_first_call = false;
  } else {
    sum_iface_.reset(nullptr);
    //    QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
    // Раскомментировать для работы через remote host:
    QDBusConnection::disconnectFromBus(connect_name_);
    output_te_->append("Disconnected from DBus.");
    id_on_dbus_label_value_->clear();
  }

  disconnect(connect_pb_, &QPushButton::clicked, this, &widget_with_interface::disconnect_from_dbus);
  connect   (connect_pb_, &QPushButton::clicked, this, &widget_with_interface::connect_to_dbus);
  connect_pb_->setText(tr("Подключиться\nк DBus"));
}


void widget_with_interface::load_settings(QLineEdit& ip_le, QLineEdit& port_le) {
  settings_ = std::make_unique<QSettings>("../config/interface.ini", QSettings::IniFormat);

  ip_le.setText(settings_->value("IP").toString());
  port_le.setText(settings_->value("port").toString());
}
