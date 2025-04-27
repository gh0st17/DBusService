#pragma once
#include <filesystem>
#include <map>
#include <memory>

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

class AppInstance {
 private:
  unique_ptr<sdbus::IObject> object;
  unique_ptr<ConnParams> cp;
  config dict;
  fs::path configPath;

  const sdbus::SignalName signalName{"configurationChanged"};

  map<string, sdbus::Variant> getConfigCallback() const;

  void setConfigCallback(const string& key, const sdbus::Variant& value);

  void readConfig();
  void writeConfig() const;

 public:
  AppInstance(const fs::path& configPath, const ConnParams& cp);
};