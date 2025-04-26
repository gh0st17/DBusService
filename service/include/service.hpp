#pragma once
#include <memory>
#include <map>

#include <sdbus-c++/sdbus-c++.h>
#include <json/json.h>

using namespace std;

class DBusService {
private:
  unique_ptr<sdbus::IObject> object;
  unique_ptr<sdbus::IConnection> conn;
  map<string, map<string, sdbus::Variant> > dict;
  vector<string> configsPaths;

  const string serviceName = "com.system.configurationManager";
  const string interfaceName = serviceName + ".Application.Configuration";
  const string signalName = "configurationChanged";
  const string objectPath = "/com/system/configurationManager/";

  void initObject(const string& configPath);

  void readConfig(const string& configPath);
  void writeConfig(const string& configPath) const;

public:
  DBusService(vector<string>&& configsPaths);

  void startService();
};