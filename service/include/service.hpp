#pragma once
#include <memory>
#include <map>

#include "app_instance.hpp"

#include <sdbus-c++/sdbus-c++.h>
#include <json/json.h>

using namespace std;

class AppInstance;
using config = map<string, sdbus::Variant>;

class DBusService {
private:
  vector< unique_ptr<AppInstance> > instances;
  unique_ptr<sdbus::IConnection> conn;

  const string serviceName = "com.system.configurationManager";
  const string interfaceName = serviceName + ".Application.Configuration";
  const string signalName = "configurationChanged";
  const string objectPath = "/com/system/configurationManager/";

  void initInstance(const fs::path& configPath);

public:
  DBusService(vector<fs::path>&& configsPaths);

  void startService();
};