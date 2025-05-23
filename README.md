# DBusService

Проект реализован в рамках выполнения тестового задания для Открытых Мобильных Платформ

## Требования к ПО

Подразумевается наличие в системе библиотек и ПО для сборки:

- Компилятор `GCC` или `Clang`
- Система сборки `CMake`
- Библиотека `jsoncpp`
- Библиотека `sdbus-c++` версии 2.1.0
	- Репозитории содержит предкомпилированные файлы в директории `lib`
- Библиотека `PkgConfig`

## Установка необходимого ПО для сборки в `Ubuntu 20.04` или `Ubuntu 22.04`

1. Откройте окно терминала
2. Выполните от имени супер-пользователя следующую команду для установки пакетов:

Для `Ubuntu 20.04`

```bash
apt install -y cmake build-essential libjsoncpp-dev libjsoncpp1 python3-pkgconfig libsystemd-dev
```

Для `Ubuntu 22.04`

```bash
apt install -y cmake build-essential libjsoncpp-dev libjsoncpp25 python3-pkgconfig libsystemd-dev
```
3. Можно перейти к разделу "Инструкция по сборке".

## Инструкция по сборке

1. Загрузите этот репозитории на диск любым из предложенных способов:
  - Загрузите как архив и распакуйте, откройте распакованную директорию в окне терминала;
  - Используйте утилиту `git`, в окне терминала выполните:

```bash
git clone https://github.com/gh0st17/DBusService.git
cd DBusService
```

2. Выполните в окне терминала команду для подготовки файлов для сборки:

```bash
cmake -G Unix\ Makefiles -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

## Инструкция по использованию

1. После сборки серверная часть сервиса запускается следующей командой:

```bash
./build/service/DBusService -p configs
```

Параметр `-p` служит для указания пути к директории с файлами конфигурации приложении.

2. После успешного запуска в окне терминала будет строка:

```bash
[  INFO ] Entering into event loop
```

означающая, что сервер готов обмениваться сообщениями по сессионной шине `DBus`.

3. В другом окне терминала из текущей директории запустите клиентскую часть командой:

```bash
./build/app/confManagerApplication -p configs
```

При успешном запуске сообщения ключи `TimeoutPhrase` каждого имитируемого приложения начнут выводиться, каждый через свой `Timeout` интервал.

4. Создаете еще одно окно терминала и запустите команду:

```bash
gdbus call --session --dest com.system.configurationManager \
  --object-path /com/system/configurationManager/Application/confManagerApplication1 \
  --method com.system.configurationManager.Application.Configuration.ChangeConfiguration \
  "TimeoutPhrase" "<'STOP'>"
```

5. Появится результат вызова метода `ChangeConfiguration`:

```
("Key 'TimeoutPhrase' was set to 'STOP'",)
```

При этом в окне с клиентской частью приложение с именем `confManagerApplication1` обновит значение ключа `TimeoutPhrase` на значение равное `STOP`.
