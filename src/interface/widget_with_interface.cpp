#include "widget_with_interface.h"

#include <vector>

#include <QComboBox>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "../shared/connector_widget.h"
#include "../shared/my_data.h"


widget_with_interface::widget_with_interface(QWidget *parent)
  : QWidget(parent) {
  QFont mono_12_db   ("Monospace", 12, QFont::DemiBold);
  QFont mono_14_db   ("Monospace", 14, QFont::DemiBold);
  QFont courier_12_db("Courier",   12, QFont::DemiBold);
  QFont arial_12     ("Arial",     12, QFont::Normal);
  setFont(arial_12);

  auto* header_label = new QLabel(tr("Интерфейс DBus (как бы клиент)"));
  header_label->setFont(mono_14_db);
  //------------------------------------------------

  w_ = new connector_widget(this);
  QColor green_color{110, 140, 60};
  w_->connect_pb_->setPalette(green_color);
  w_->dbus_registration_label_->setText("DBus id:");
  //------------------------------------------------

  auto* output_label = new QLabel(tr("Логи подключения + данные, полученные от adaptor"));
  output_label->setFixedHeight(35);
  output_te_ = new QTextEdit();
  output_te_->setFont(courier_12_db);
  output_te_->setMinimumSize(220, 350);

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
  grid->addWidget(header_label,      0, 0,   2, 5, Qt::AlignHCenter | Qt::AlignTop);
  grid->addWidget(w_,                2, 0,   4, 5);
  grid->addWidget(output_label,      6, 0,   1, 5, Qt::AlignHCenter | Qt::AlignBottom);
  grid->addWidget(output_te_,        7, 0,  18, 5);
  grid->addLayout(void_method_lyt,  26, 2,   2, 3);
  grid->addLayout(sum_lyt,          28, 2,   2, 3);
  setLayout(grid);

  connect(w_->dbus_type_cb_, &QComboBox::currentIndexChanged, [this](int index) {
    index == 0 ? w_->settings_w_->hide() : w_->settings_w_->show();
    disconnect_from_dbus();
  });
  emit w_->dbus_type_cb_->currentIndexChanged(0);

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

  connect(void_method_remote_call_pb, &QPushButton::clicked,
          [this, first_member_le, second_member_le, third_member_le]() {
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


void widget_with_interface::connect_to_dbus() {
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
    output_te_->append("  Interface registerd");
    // При конструировании Интерфейса необходимо задать переменную path.
    // Значение переменной path должно совпадать со значением, указанным на стороне Адаптора
    sum_iface_ = std::make_unique<org::rumba::Sum>(org::rumba::Sum::staticInterfaceName(),
                                                   object_path_, // "/Sum"
                                                   *current_conn_,
                                                   this);
    w_->dbus_registration_label_value_->setText("\"" + sum_iface_->connection().baseService() + "\"");
    connect(sum_iface_.get(), &OrgRumbaSumInterface::bool_data_signal, [this](bool val) {
      output_te_->append(val ? "true" : "false");
    });

    w_->connect_pb_->setText(w_->connect_pb_names_.second);
    disconnect(w_->connect_pb_, &QPushButton::clicked, this, &widget_with_interface::connect_to_dbus);
    connect   (w_->connect_pb_, &QPushButton::clicked, this, &widget_with_interface::disconnect_from_dbus);
  } else {
    w_->connect_pb_->setChecked(false);
  }
}


void widget_with_interface::disconnect_from_dbus() {
  if (w_->connect_pb_->text() != w_->connect_pb_names_.first) {
    w_->connect_pb_->setText(w_->connect_pb_names_.first);
    disconnect(w_->connect_pb_, &QPushButton::clicked, this, &widget_with_interface::disconnect_from_dbus);
    connect   (w_->connect_pb_, &QPushButton::clicked, this, &widget_with_interface::connect_to_dbus);
  }

  if (w_->connect_pb_->isChecked()) {
    w_->connect_pb_->setChecked(false);
  }

  if (current_conn_ && current_conn_->isConnected() &&
      !w_->dbus_registration_label_value_->text().isEmpty()) {
    sum_iface_.reset(nullptr);
    output_te_->append("Disconnected (interface closed)");
    w_->dbus_registration_label_value_->clear();
  }
}
