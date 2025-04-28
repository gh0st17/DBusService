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
  list<unique_ptr<AppInstance> > instances;
  unique_ptr<sdbus::IConnection> conn;

  /*
   * Параметры сервиса
   */

  const sdbus::ServiceName serviceName{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName{sdbus::InterfaceName(
    static_cast<string>(serviceName) + ".Application.Configuration")};
  const sdbus::SignalName signalName{"configurationChanged"};
  const sdbus::ObjectPath objectPath{"/com/system/configurationManager/"};

  /// @brief Запускает одну сущность приложения `AppInstance`
  /// @param configPath Путь к конфигурационному файлу приложения
  void initInstance(const fs::path& configPath);

 public:
  DBusService(vector<fs::path>&& configsPaths);

  /// @brief Запускает сервис
  void startService();
};