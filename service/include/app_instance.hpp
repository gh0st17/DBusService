#pragma once
#include <filesystem>
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

  const sdbus::SignalName signalName_{"configurationChanged"};

  /// @brief Обработчик для метода `GetConfiguration`
  /// @return Функцию-обработчик
  sdbus::method_callback getConfigCallback() const;

  /// @brief Определяет обработчик для метода `ChangeConfiguration`
  /// @return Функцию-обработчик
  sdbus::method_callback setConfigCallback();

  /// @brief Читает конфигурацию из файла
  ///        по пути `configPath_` в `dict_`
  void readConfig();

  /// @brief Записывает конфигурацию из `dict_`
  ///        в файл по пути `configPath_`
  void writeConfig();

 public:
  AppInstance(const fs::path& configPath, const ConnParams& cp);
};