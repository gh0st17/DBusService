#pragma once
#include <filesystem>
#include <memory>
#include <map>

#include <sdbus-c++/sdbus-c++.h>

using namespace std;
namespace fs = std::filesystem;

using config = map<string, sdbus::Variant>;

struct ConnParams {
  sdbus::IConnection& conn;
  string interfaceName;
  string objectPath;
};

class AppInstance {
private:
  unique_ptr<sdbus::IObject> object;
  sdbus::Signal signal;
  unique_ptr<ConnParams> cp;
  config dict;
  fs::path configPath;

  const string signalName = "configurationChanged";

  map<string, sdbus::Variant> getConfigCallback() const;
  
  void setConfigCallback(
    const string& key,
    const sdbus::Variant& value
  );

  void readConfig(const string& configPath);
  void writeConfig(const string& configPath) const;

public:
  AppInstance(const fs::path& configPath, const ConnParams& cp);
};