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


WidgetWithInterface::WidgetWithInterface(QWidget *parent)
    : QWidget(parent)
{
    QFont mono_12db("Monospace", 12, QFont::DemiBold);
    QFont mono_14db("Courier 10 Pic", 14, QFont::DemiBold); // TODO: rename
    _courier = new QFont("Courier", 12, QFont::DemiBold);
    _arial_12 = new QFont("Arial", 12, QFont::Normal);
    setFont(*_arial_12);
    // Поля ввода сетевых настроек:
    _lb03 = new QLabel(tr("Настройки для соединения с пользовательской шиной Dbus:"));
//                           "При незаполненных полях инициируется "
//                           "соединение с локальной\nсессионной шиной."
    _lbIp = new QLabel(tr("IP-адрес:"));
    _lbPort = new QLabel(tr("Tcp-порт:"));
    _lb04 = new QLabel(tr("ID клиента на шине DBus:"));
    _lbConnName = new QLabel();
    _leIp = new QLineEdit();
    _leIp->setFixedWidth(180);
    _lePort = new QLineEdit();
    _lePort->setFixedWidth(180);
    usePreviousSettings(_leIp, _lePort);

    _pbConnect = new QPushButton();
    _pbConnect->setFixedSize(180, 70);
//    QColor colorOrange(210, 120, 34);
    QColor colorGreen1(110,140,60);
    _pbConnect->setPalette(colorGreen1);
    // Прочие поля:
    _teOutput = new QTextEdit();
    _teOutput->setFont(*_courier);
    _teOutput->setMinimumSize(220, 350);
    _lb01 = new QLabel(tr("Интерфейс Dbus (как бы клиент)"));
    _lb01->setFont(mono_14db);
    _lb02 = new QLabel(tr("Логи подключения + данные, полученные от adaptorApp"));
    _lb02->setFixedHeight(35);
    _pbSendByMethod = new QPushButton(tr("Отправить данные,\nиспользуя метод <method>"));
    _pbSendByMethod->setFixedHeight(80);
    _pbSendByMethod->setPalette(colorGreen1);

    _grid = new QGridLayout(this);
    _grid->addWidget(_lb01,     0, 0,   1, 4, Qt::AlignCenter);
    _grid->addWidget(_lb03,     1, 0,   1, 4, Qt::AlignHCenter);
    _grid->addWidget(_lbIp,     2, 0,   1, 1, Qt::AlignRight);
    _grid->addWidget(_leIp,     2, 1,   1, 1);
    _grid->addWidget(_pbConnect,   2, 2,   2, 2, Qt::AlignRight);
    _grid->addWidget(_lbPort,      3, 0,   1, 1, Qt::AlignRight);
    _grid->addWidget(_lePort,      3, 1,   1, 1);
    _grid->addWidget(_lb04,        4, 0,   1, 1, Qt::AlignRight | Qt::AlignTop);
    _grid->addWidget(_lbConnName,  4, 1,   1, 1, Qt::AlignTop);

    _grid->addWidget(_lb02,     5, 0,   1, 4, Qt::AlignHCenter | Qt::AlignBottom);
    _grid->addWidget(_teOutput, 6, 0,   8, 4);
    _grid->addWidget(_pbSendByMethod,  15, 2, 2, 2);
    setLayout(_grid);

    disconnectFromDBus();
    connect(_pbSendByMethod, SIGNAL(clicked()),  this, SLOT(useMethod()) );
}

WidgetWithInterface::~WidgetWithInterface()
{
    _settings->setValue("IP", _leIp->text());
    _settings->setValue("port", _lePort->text());
    delete _settings;
}


void WidgetWithInterface::useMethod()
{
    if (_sum == nullptr)
        return;
    static int b = 0;
    b++;
    if (b%2 == 0)
        _sum->methodVoid(10, 12);
    else
    {
        MyData data(2.01, 0.13, 1);
        QDBusPendingReply<double> reply = _sum->methodWithReturnValue(data, 2002);
        reply.waitForFinished();
        if (reply.isError())
            _teOutput->append(reply.error().message());
        else
            _teOutput->append("Summa = " + QString::number(reply.value()) );
    }
}

void WidgetWithInterface::usePreviousSettings(
        QLineEdit *leForIp, QLineEdit *leForPort)
{
    _settings = new QSettings("../config/interfaceApp.ini", QSettings::IniFormat);
    QString ip = _settings->value("IP").toString();
    leForIp->setText(ip);
    QString port = _settings->value("port").toString();
    leForPort->setText(port);
}

//void WidgetWithInterface::timerEvent(QTimerEvent *event)
//{}

void WidgetWithInterface::slotForFirst(bool val)
{
    if (val == true)
        _teOutput->append("true");
    else
        _teOutput->append("false");
}

void WidgetWithInterface::connectToDBus()
{
    QString path = "tcp:host=" + _leIp->text() + ",port=" + _lePort->text();
    qDebug() << path;

    auto connection = QDBusConnection::sessionBus();
    // Раскомментировать для работы через remote host:
//    auto connection = QDBusConnection::connectToBus(path, _connName);

    if (connection.isConnected())
    {
        _teOutput->append("Connection to DBus with " + _leIp->text() + ":" + _lePort->text());
        _teOutput->append("Success connection.");
        // При конструировании Интерфейса необходимо задать переменную path.
        // Значение переменной path должно совпадать со значением, указанным
        // на стороне Адаптора.
        _sum = std::make_unique<org::rumba::Sum>(org::rumba::Sum::staticInterfaceName()
                                   , _objectPath //"/Sum"
                                   , connection
                                   , this);

//        qDebug() << _sum->connection().baseService();
        _lbConnName->setText("\"" + _sum->connection().baseService() + "\"");
        connect(_sum.get(), SIGNAL(signalFirst(bool)), this, SLOT(slotForFirst(bool)) );

        _pbConnect->setText(tr("Отсоединиться\nот DBus"));
        disconnect(_pbConnect, SIGNAL(clicked()),   this, SLOT(connectToDBus()) );
        connect(_pbConnect, SIGNAL(clicked()),   this, SLOT(disconnectFromDBus()) );
//        this->startTimer(1000);
    }
    else
    {
        _teOutput->append("Connection failed!");
        QDBusConnection::disconnectFromBus(_connName);
    }
}

void WidgetWithInterface::disconnectFromDBus()
{
    static bool isFirstCall{true};
    if (isFirstCall == true)
        isFirstCall = false;
    else
    {
        _sum.reset(nullptr);
        //    QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
        // Раскомментировать для работы через remote host:
        QDBusConnection::disconnectFromBus(_connName);
        _teOutput->append("Disconnected from DBus.");
        _lbConnName->clear();
    }

    disconnect(_pbConnect, SIGNAL(clicked()),   this, SLOT(disconnectFromDBus()) );
    connect(_pbConnect, SIGNAL(clicked()),   this, SLOT(connectToDBus()) );
    _pbConnect->setText(tr("Подключиться\nк DBus"));
}


