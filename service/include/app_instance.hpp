#pragma once
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "generic/logger.hpp"
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
  shared_ptr<ConnParams> cp_;
  fs::path configPath_;
  Logger& logger_ = Logger::getInstance();
  string appName_;
  config dict_;
  mutex mu_;

  /// @brief Обработчик для метода `GetConfiguration`
  /// @return Функцию-обработчик
  sdbus::method_callback getConfigCallback();

  /// @brief Оборачивает функцию `setConfigHandler` в обработчик ошибок
  /// @return Функцию-обработчик
  sdbus::method_callback setConfigCallback();

  /// @brief Определяет обработчик для метода `ChangeConfiguration`
  /// @param call Объект вызова метода
  void setConfigHandler(sdbus::MethodCall call);

  /// @brief Проверяет совпадает ли тип нового значения
  //         `value` ключа `key` или нет
  /// @param key Ключ для проверки
  /// @param value Проверяемое значение
  /// @return `true` если типы совпадают, иначе - `false`
  const bool isTypeMatches(const string& key,
                           const sdbus::Variant& value) const;

  /// @brief Возвращает ответ на вызов метода
  /// @param call Объект вызова метода
  /// @param message Сообщения для ответа метода `DBus`
  void replyAnswer(sdbus::MethodCall& call, const string& message);

  /// @brief Читает конфигурацию из файла
  ///        по пути `configPath_` в `dict_`
  void readConfig();

  /// @brief Записывает конфигурацию из `dict_`
  ///        в файл по пути `configPath_`
  void writeConfig();

 public:
  AppInstance(const fs::path& configPath, const shared_ptr<ConnParams> cp);
};