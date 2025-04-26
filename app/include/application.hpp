#pragma once
#include <map>
#include <memory>

#include <sdbus-c++/sdbus-c++.h>
#include <json/json.h>

using namespace std;

using config = map<string, sdbus::Variant>;

class ConfManagerApplication {
private:
  std::unique_ptr<sdbus::IProxy> proxy;
  config dict;

  void readConfig(const string& configPath);
  void writeConfig(const string& configPath) const;

public:
  ConfManagerApplication(const string& configPath);
};