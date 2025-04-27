#pragma once
#include <map>
#include <mutex>
#include <memory>
#include <filesystem>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

using config = map<string, sdbus::Variant>;

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
  unique_ptr<sdbus::IObject> object;
  unique_ptr<ConnParams> cp;
  fs::path configPath;
  config dict;
  mutex mu;

  const sdbus::SignalName signalName{"configurationChanged"};
  
  /// @brief Обработчик для метода `GetConfiguration`
  /// @return Словарь с параметрами приложения
  map<string, sdbus::Variant> getConfigCallback() const;

  /// @brief Обработчик для метода `ChangeConfiguration`
  /// @param key Имя параметра для изменения
  /// @param value Новое значение параметра
  void setConfigCallback(const string& key, const sdbus::Variant& value);

  /// @brief Читает конфигурацию из файла по пути `configPath` в `dict`
  void readConfig();

  /// @brief Записывает конфигурацию из `dict` в файл по пути `configPath`
  void writeConfig();

 public:
  AppInstance(const fs::path& configPath, const ConnParams& cp);
};