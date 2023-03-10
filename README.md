# Пример работы через шину Dbus

Организация взаимодействия разных частей ПО (бизнес-процессов, модулей, монитора) с использованием шины DBus.

## Тестовый пример № 1
В рамках данного примера реализовано взаимодействие двух процессов через шину DBus. Взаимодействие осуществляется с использованием библиотеки Qt.
**Взаимодействие "точка-точка" через шину Dbus в терминологии Qt предполагает следующее разделение ролей:**
- `"адаптор"` соединяется с шиной и регистрирует уникальное имя сервиса (пример: org.rumba.Sum) и объект (объект по своей структуре похож на путь в файловой системе). Уникальное имя сервиса не может быть повторно зарегистрировано другим приложением (оно "захватывается" также, как tcp-порт, например);
- `"интерфейс"` соединяется с шиной и регистрирует аналогичный объект (путь). При конструировании объекта типа `"интерфейс"` ему передается, в числе прочего, имя сервиса.
Таким образом, интерфейс может "писать сообщения" в адрес адаптора.

Передача сообщения от интерфейса к адаптору очень похожа на вызов удаленной процедуры.
Интерфейс знает сигнатуру метода (имя, входные параметры и возвращаемое значение) и осуществляет вызов с необходимыми аргументами. Вызов метода осуществляется на стороне адаптора. Возвращаемое значение передается в адрес интерфейса.
Со стороны адаптора существует возможность отправить сигнал в адрес интерфейса. Сигнатура сигнала также может содержать входные параметры.

**С технической точки зрения, взаимодействие двух приложений в Qt организовано следующим образом:**
1. Создается общий xml-файл, в котором описываются соответствующие методы и сигналы.
2. В .pro-файлы проектов добавляются директивы **DBUS_ADAPTORS** и **DBUS_INTERFACES**. Данные директивы необходимы для осуществления автогенерации вспомогательных классов `MyRegNameInterface` и `MyRegNameAdaptor` из xml-файла. Наличие единого xml-файла позволяет изменять интерфейс взаимодействия между приложениями в одном месте. Это, конечно, не отменяет необходимости вносить правки в код клиентских классов, но упрощает общий процесс изменения интерфейса.

3. Для интерфейса:
- создаем клиентский класс, в котором создаем объект класса `MyRegNameInterface`;
- соединяемся с шиной DBus с помощью статического метода QDBusConnection::connectToBus();
- теперь можно вызывать методы объекта класса `MyRegNameInterface` и связывать клиентские слоты с сигналами класса `MyRegNameInterface`.
4. Для адаптора:
- создаем клиентский класс, в котором объявляем и определяем те же сигналы и слоты, которые содержатся в классе `MyRegNameAdaptor`;
- создаем объект класса `MyRegNameAdaptor` (в его конструктор передаем указатель на пользовательский класс). Таким образом происходит "связывание" сигналов и слотов клиентского класса с соответсвующими сигналами и слотами класса `MyRegNameAdaptor`;
- теперь можно ожидать "удаленных вызовов" слотов и отправлять сигналы.

**Запуск пользовательской шины DBus**\
В ОС Linux, как правило, запущены две шины DBus: системная и сессионная.\
Приложения, разработанные в рамках данного примера, взаимодействуют между собой через **пользовательскую шину**. Данная шина будет настроена на прослушивание удаленных подключений, что позволит разнести приложения по разным хостам.\
Порядок запуска пользовательской шины DBus в ОС Astra Linux 1.6:
1. Скопировать файл /usr/share/dbus-1/session.conf в любую директорию;
2. Исправить скопированный файл следующим образом:\
перед строкой
```cpp
<listen>unix:tmpdir=/tmp</listen>
```
добавить строку
```cpp
<listen>tcp:host=localhost,bind=*,port=ПОРТ,family=ipv4</listen>
```
, далее добавить строки
```cpp
<auth>ANONYMOUS</auth>
<allow_anonymous/>
```
3. Запустить демона 'dbus-daemon` с помощью следующей команды:
```cpp
dbus-daemon --fork --config-file=/путь/до/session.conf
```




