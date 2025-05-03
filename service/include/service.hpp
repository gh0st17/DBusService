#pragma once
#include <json/json.h>

#include <list>
#include <map>
#include <memory>

#include "app_instance.hpp"
#include "sdbus-c++/sdbus-c++.h"

using namespace std;

/// @brief Класс для представления `DBus` сервиса
class DBusService {
 private:
  list<unique_ptr<AppInstance> > instances_;
  unique_ptr<sdbus::IConnection> conn_;

  /*
   * Параметры сервиса
   */

  const sdbus::ServiceName serviceName_{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName_{sdbus::InterfaceName(
    static_cast<string>(serviceName_) + ".Application.Configuration")};
  const sdbus::SignalName signalName_{"configurationChanged"};
  const sdbus::ObjectPath objectPath_{"/com/system/configurationManager/"};

  /// @brief Запускает одну сущность приложения `AppInstance`
  /// @param configPath Путь к конфигурационному файлу приложения
  void initInstance(const fs::path& configPath);

 public:
  DBusService(vector<fs::path>&& configsPaths);

  /// @brief Запускает сервис
  void start();

  /// @brief Останавливает сервис
  void stop();
};