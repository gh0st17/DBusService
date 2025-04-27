#pragma once
#include <json/json.h>

#include <map>
#include <memory>

#include "app_instance.hpp"
#include "sdbus-c++/sdbus-c++.h"

using namespace std;

class AppInstance;
using config = map<string, sdbus::Variant>;

class DBusService {
 private:
  vector<unique_ptr<AppInstance> > instances;
  unique_ptr<sdbus::IConnection> conn;

  const sdbus::ServiceName serviceName{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName{sdbus::InterfaceName(
    static_cast<string>(serviceName) + ".Application.Configuration")};
  const sdbus::SignalName signalName{"configurationChanged"};
  const sdbus::ObjectPath objectPath{"/com/system/configurationManager/"};

  void initInstance(const fs::path& configPath);

 public:
  DBusService(vector<fs::path>&& configsPaths);

  void startService();
};