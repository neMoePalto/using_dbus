#include "widget_with_interface.h"

#include <vector>

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
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

  auto* sum_remote_call_pb = new QPushButton(tr("Запросить расчет \nсуммы чисел на адапторе,"
                                                "\n(remote call метода с return value)"));
  sum_remote_call_pb->setFixedHeight(80);
  sum_remote_call_pb->setPalette(green_color);
  auto* first_sum_le = new QLineEdit();
  first_sum_le->setFixedSize(60, 40);
  auto* second_sum_le = new QLineEdit();
  second_sum_le->setFixedSize(60, 40);
  auto* plus_symbol_label = new QLabel("+");

  auto* sum_lyt = new QHBoxLayout();
  sum_lyt->addStretch();
  sum_lyt->addWidget(first_sum_le);
  sum_lyt->addWidget(plus_symbol_label);
  sum_lyt->addWidget(second_sum_le);
  sum_lyt->addWidget(sum_remote_call_pb);

  auto* void_method_remote_call_pb = new QPushButton(tr("Отправить объект \nmy_data на адаптор,"
                                                        "\n(remote call void-метода)"));
  void_method_remote_call_pb->setFixedHeight(80);
  void_method_remote_call_pb->setPalette(green_color);
  auto* first_member_le = new QLineEdit();
  first_member_le->setFixedSize(60, 40);
  auto* second_member_le = new QLineEdit();
  second_member_le->setFixedSize(60, 40);
  auto* third_member_le = new QLineEdit();
  third_member_le->setFixedSize(60, 40);

  auto* void_method_lyt = new QHBoxLayout();
  void_method_lyt->addStretch();
  void_method_lyt->addWidget(first_member_le);
  void_method_lyt->addWidget(second_member_le);
  void_method_lyt->addWidget(third_member_le);
  void_method_lyt->addWidget(void_method_remote_call_pb);

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
  grid->addLayout(void_method_lyt,         15, 1,   2, 3);
  grid->addLayout(sum_lyt,                 17, 1,   2, 3);
  setLayout(grid);

  disconnect_from_dbus();
  connect(sum_remote_call_pb, &QPushButton::clicked, [this, first_sum_le, second_sum_le]() {
    if (sum_iface_ == nullptr) {
      return;
    }

    std::vector<int> values;
    int ch = 0;
    for (auto* le : {first_sum_le, second_sum_le}) {
      bool ok;
      ++ch;
      int val = le->text().toInt(&ok);
      if (ok) {
        values.push_back(val);
      } else {
        output_te_->append("Bad " + QString::number(ch) + "-th arg, check it.");
        return;
      }
    }
    QDBusPendingReply<int> reply = sum_iface_->get_sum_method(values.at(0), values.at(1));
    reply.waitForFinished();
    QString text = reply.isError() ? reply.error().message()
                                   : "Summa = " + QString::number(reply.value());
    output_te_->append(text);
  });

  connect(void_method_remote_call_pb, &QPushButton::clicked, [this,
          first_member_le, second_member_le, third_member_le]() {
    if (sum_iface_ == nullptr) {
      return;
    }

    std::vector<double> values;
    int ch = 0;
    for (auto* le : {first_member_le, second_member_le, third_member_le}) {
      bool ok;
      ++ch;
      double val = le->text().toDouble(&ok);
      if (ok) {
        values.push_back(val);
      } else {
        output_te_->append("Bad " + QString::number(ch) + "-th arg, check it.");
        return;
      }
    }
    sum_iface_->void_method(my_data{values.at(0), values.at(1), values.at(2)}, 123.045);
  });
}


widget_with_interface::~widget_with_interface() {
  settings_->setValue("IP", ip_le_->text());
  settings_->setValue("port", port_le_->text());
}


void widget_with_interface::connect_to_dbus() {
  QString path = "tcp:host=" + ip_le_->text() + ",port=" + port_le_->text();

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
