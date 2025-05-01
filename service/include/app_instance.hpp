#pragma once
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

/// @brief Псевдоним для типа конфигурации
using config = map<string, sdbus::Variant>;

/// @brief Структура для передачи параметров
struct ConnParams {
  sdbus::IConnection& conn;
  sdbus::InterfaceName interfaceName;
  sdbus::SignalName signalName;
  sdbus::ObjectPath objectPath;
};

/// @brief Класс для представления приложения
///        как сущности `DBus`
class AppInstance {
 private:
  unique_ptr<sdbus::IObject> object_;
  unique_ptr<ConnParams> cp_;
  fs::path configPath_;
  config dict_;
  mutex mu_;

  /// @brief Обработчик для метода `GetConfiguration`
  /// @return Функцию-обработчик
  sdbus::method_callback getConfigCallback();

  /// @brief Определяет обработчик для метода `ChangeConfiguration`
  /// @return Функцию-обработчик
  sdbus::method_callback setConfigCallback();

  /// @brief Псевдоним для функции обработки сообщения об ошибке
  using ErrFunc = const function<void(const string&)>&;

  /// @brief Проверяет существование ключа в `dict_`
  /// @param key Ключ для проверки
  /// @return `true` если существует, иначе - `false`
  bool isKeyExists(const string& key) const;

  /// @brief Проверяет совпадает ли тип нового значения
  //         `value` ключа `key` или нет
  /// @param key Ключ для проверки
  /// @param value Проверяемое значение
  /// @return `true` если типы совпадают, иначе - `false`
  bool isTypeMatches(const string& key, const sdbus::Variant& value) const;
  
  /// @brief Выполняет сохранение конфигурации в файл
  ///        с обработкой возможных ошибок
  /// @param handleError Функция обработки сообщения об ошибке
  /// @return `true` если запись в файл успешна, иначе - `false`
  bool writeConfigSafely(ErrFunc handleError);
  
  /// @brief Излучает сигнал об изменении конфигурации для
  ///        соответсвующего приложения
  /// @param key Ключ, новое значение которого нужно передать по шине `DBus`
  /// @param handleError Функция обработки сообщения об ошибке
  /// @return `true` если сигнал отправлен успешно, иначе - `false`
  bool emitConfigChangedSignal(const string& key, ErrFunc handleError);

  /// @brief Читает конфигурацию из файла
  ///        по пути `configPath_` в `dict_`
  void readConfig();

  /// @brief Записывает конфигурацию из `dict_`
  ///        в файл по пути `configPath_`
  void writeConfig();

 public:
  AppInstance(const fs::path& configPath, const ConnParams& cp);
};