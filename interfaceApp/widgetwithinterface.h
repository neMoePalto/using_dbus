#ifndef WIDGETWITHINTERFACE_H
#define WIDGETWITHINTERFACE_H
#include <QWidget>
#include <memory>
#include "summa_interface.h"

class QLabel;
class QLineEdit;
class QTextEdit;
class QGridLayout;
class QPushButton;
class WidgetWithInterface : public QWidget
{
    Q_OBJECT
public:
    WidgetWithInterface(QWidget *parent = nullptr);
    ~WidgetWithInterface();
//protected:
//    void timerEvent(QTimerEvent *event) override;
public slots:
    void slotForFirst(bool val);
private slots:
    void useMethod();
    void connectToDBus();
    void disconnectFromDBus();
private:
    // Интерфейс DBus, используется для отправки сообщений:
    std::unique_ptr<org::rumba::Sum> _sum;

    QGridLayout*    _grid;
    QTextEdit*      _teOutput;
    QPushButton*    _pbSendByMethod;
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
    const QString _objectPath{"/"};
    QFont*  _courier;
    QFont*  _arial_12;

    QSettings* _settings;
    void usePreviousSettings(QLineEdit* leForIp, QLineEdit* leForPort);
};

#endif // WIDGETWITHINTERFACE_H
