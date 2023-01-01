#include "widget_with_adaptor.h"

#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "summa_adaptor.h"


WidgetWithAdaptor::WidgetWithAdaptor(QWidget *parent)
    : QWidget(parent)
{
    QFont mono_12db("Monospace", 12, QFont::DemiBold);
    QFont mono_14db("Monospace", 14, QFont::DemiBold);
    _courier = new QFont("Courier", 12, QFont::DemiBold);
    _arial_12 = new QFont("Arial", 12, QFont::Normal);
    setFont(*_arial_12);
    // Поля ввода сетевых настроек:
    _lb03 = new QLabel(tr("Настройки для соединения с пользовательской шиной Dbus:"));
//                           "При незаполненных полях инициируется "
//                           "соединение с локальной\nсессионной шиной."));
    _lbIp = new QLabel(tr("IP-адрес:"));
    _lbPort = new QLabel(tr("Tcp-порт:"));
    _lb04 = new QLabel(tr("Зарегистрирован сервис:"));
    _lbConnName = new QLabel();
    _leIp = new QLineEdit();
    _leIp->setFixedWidth(180);
    _lePort = new QLineEdit();
    _lePort->setFixedWidth(180);
    usePreviousSettings(_leIp, _lePort);

    _pbConnect = new QPushButton();
    _pbConnect->setFixedSize(180, 70);
    QColor colorOrange(210, 120, 34);
    _pbConnect->setPalette(colorOrange);
    // Прочие поля:
    _teOutput = new QTextEdit();
    _teOutput->setFont(*_courier);
    _teOutput->setMinimumSize(220, 350);
    _lb01 = new QLabel(tr("Адаптор Dbus (как бы сервер)"));
    _lb01->setFont(mono_14db);
    _lb02 = new QLabel(tr("Логи подключения + данные, полученные от interfaceApp"));
    _lb02->setFixedHeight(35);
    _pbSendBySignal = new QPushButton(tr("Отправить данные в адрес всех\nподключенных"
                                         " к сервису клиентов,\nиспользуя сигнал <signal>"));
    _pbSendBySignal->setFixedHeight(80);
    _pbSendBySignal->setPalette(colorOrange);

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
    _grid->addWidget(_pbSendBySignal, 15, 0, 2, 2);
    setLayout(_grid);

    disconnectFromDBus();
    connect(_pbSendBySignal, SIGNAL(clicked()), this, SLOT(useSignal()) );
}

WidgetWithAdaptor::~WidgetWithAdaptor()
{
    _settings->setValue("IP", _leIp->text());
    _settings->setValue("port", _lePort->text());
    delete _settings;
}

double WidgetWithAdaptor::methodWithReturnValue(MyData value01, int value02)
{
    auto result = value01.makeSum();
    _teOutput->append(QObject::tr("Сумма значений членов класса = ") + QString::number(result));
//    signalFirst(true);
//    QThread::sleep(3);
    return result;
}

void WidgetWithAdaptor::methodVoid(int valInt01, int valInt02)
{
    _teOutput->append(QString::number(valInt01) + " and " +
                      QString::number(valInt02) + ", no return value");
//    signalFirst(false);
}

void WidgetWithAdaptor::useSignal()
{
    static int ch = 0;
    ch++;
    if (ch%2 == 0)
        signalFirst(true);
    else
        signalFirst(false);
}

void WidgetWithAdaptor::usePreviousSettings(
        QLineEdit *leForIp, QLineEdit *leForPort)
{
    _settings = new QSettings("../config/adaptorApp.ini", QSettings::IniFormat);
    QString ip = _settings->value("IP").toString();
    leForIp->setText(ip);
    QString port = _settings->value("port").toString();
    leForPort->setText(port);
}


void WidgetWithAdaptor::connectToDBus()
{
    // 1. Связываем класс-клиент с Адаптором:
    auto ptr = new SumAdaptor(this);
    Q_UNUSED(ptr);
    QString path = "tcp:host=" + _leIp->text() + ",port=" + _lePort->text();
    auto connection = QDBusConnection::sessionBus();
    // Раскомментировать для работы через remote host:
//    auto connection = QDBusConnection::connectToBus(path, _connName);

    if (connection.isConnected())
    {
        _teOutput->append("Connection to DBus with " + _leIp->text() + ":" + _lePort->text());
        // 2. Регистрируем имя сервиса.
        // Имя сервиса должно совпадать с именем интерфейса, который
        // описан в xml-файле.
        bool regService = connection.registerService(_serviceName);
        if (!regService)
        {
            _teOutput->append("Error: Can't register service " + _serviceName +
                              ".\nMay be, this name was already registered.");
        }
        else
        {
            _teOutput->append("Success connection.");
            _lbConnName->setText(_serviceName);
        }
        // 3. Регистрируем объект (часто имя объекта - последняя часть имени сервиса).
        // Пока не совсем ясно, почему это необходимо делать явно.
        connection.registerObject(_objectPath, this);
        _connection = std::move(connection);
//        qDebug() << regService << "and" << regObj;

        _pbConnect->setText(tr("Отсоединиться\nот DBus"));
        disconnect(_pbConnect, SIGNAL(clicked()),   this, SLOT(connectToDBus()) );
        connect(_pbConnect, SIGNAL(clicked()),   this, SLOT(disconnectFromDBus()) );
    }
    else
    {
        _teOutput->append("Connection failed!");
        QDBusConnection::disconnectFromBus(_connName);
    }
}

void WidgetWithAdaptor::disconnectFromDBus()
{
    static bool isFirstCall{true};
    if (isFirstCall == true)
        isFirstCall = false;
    else
    {
        _connection.unregisterObject(_objectPath);
        _connection.unregisterService(_serviceName);
//        _connection.~QDBusConnection();
//        QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
        // Раскомментировать для работы через remote host:
        QDBusConnection::disconnectFromBus(_connName);
        _teOutput->append("Disconnected from DBus.");
        _lbConnName->clear();
    }

    disconnect(_pbConnect, SIGNAL(clicked()),   this, SLOT(disconnectFromDBus()) );
    connect(_pbConnect, SIGNAL(clicked()),   this, SLOT(connectToDBus()) );
    _pbConnect->setText(tr("Подключиться\nк DBus"));
}

