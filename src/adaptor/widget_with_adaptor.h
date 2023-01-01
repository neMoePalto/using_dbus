#pragma once

#include <QDBusConnection>
#include <QSettings>
#include <QWidget>

#include "../shared/my_data.h"


// Класс-клиент, использующий адаптор Dbus (Адаптор)
class QLabel;
class QLineEdit;
class QTextEdit;
class QGridLayout;
class QPushButton;
class WidgetWithAdaptor : public QWidget
{
    Q_OBJECT
public:
    WidgetWithAdaptor(QWidget *parent = nullptr);
    ~WidgetWithAdaptor();
    // В отличие от Интерфейса, генерирующего свои слоты по содержимому
    // xml-файла, в классе-клиенте, работающем через Адаптор, их нужно
    // прописывать руками:
public Q_SLOTS:
    void methodVoid(int valInt01, int valInt02);
    double methodWithReturnValue(MyData value01, int value02);
Q_SIGNALS:
    void signalFirst(bool);
    // Слоты, не связанные с Адаптором:
private Q_SLOTS:
    void useSignal();
    void connectToDBus();
    void disconnectFromDBus();
private:
    QGridLayout*    _grid;
    QTextEdit*      _teOutput;
    QPushButton*    _pbSendBySignal;
    QLabel* _lb01;
    QLabel* _lb02;
    // Поля для ввода сетевых настроек:
    QLabel* _lb03;
    QLabel* _lb04;
    QLabel* _lbConnName;

    QLabel* _lbIp;
    QLabel* _lbPort;
    QLineEdit*      _leIp;
    QLineEdit*      _lePort;
    QPushButton*    _pbConnect;

    const QString _connName{"myConnection"};
    const QString _serviceName{"org.rumba.Sum"};
    const QString _objectPath{"/"};
    QDBusConnection _connection{""}; // ЗРЯ ТАСКАЮ ЗА СОБОЙ. Убрать!
    QFont*  _courier;
    QFont*  _arial_12;

    QSettings* _settings;
    void usePreviousSettings(QLineEdit* leForIp, QLineEdit* leForPort);
};
