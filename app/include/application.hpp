#pragma once
#include <any>
#include <map>
#include <mutex>
#include <memory>
#include <optional>
#include <filesystem>

#include "sdbus-c++/sdbus-c++.h"
#include <json/json.h>

using namespace std;
namespace fs = std::filesystem;

using config = map<string, any>;

class ConfManagerApplication {
private:
  unique_ptr<sdbus::IConnection> conn;
  unique_ptr<sdbus::IProxy> proxy;
  fs::path configPath;
  config dict;

  mutex mu;

  const sdbus::ServiceName serviceName{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName{
    sdbus::InterfaceName(
      static_cast<string>(serviceName) + ".Application.Configuration"
    )
  };
  const sdbus::ObjectPath objectPath{"/com/system/configurationManager/Application/"};
  const sdbus::SignalName signalName{"configurationChanged"};

  void readConfig();
  sdbus::signal_handler signalCallback();

public:
  ConfManagerApplication(const fs::path& configPath);
  ConfManagerApplication(const ConfManagerApplication&) = delete;
  ConfManagerApplication(ConfManagerApplication&& other) noexcept
    : conn(std::move(other.conn)),
      proxy(std::move(other.proxy)),
      configPath(std::move(other.configPath)),
      dict(std::move(other.dict)),
      serviceName(other.serviceName),
      interfaceName(other.interfaceName),
      objectPath(other.objectPath),
      signalName(other.signalName) {}

  ConfManagerApplication& operator=(const ConfManagerApplication&) = delete;

  ConfManagerApplication& operator=(ConfManagerApplication&& other) noexcept {
    if (this != &other) {
      conn = std::move(other.conn);
      proxy = std::move(other.proxy);
      configPath = std::move(other.configPath);
      dict = std::move(other.dict);
    }
    return *this;
  }

  void start();
  void printTimeoutPhrase();
  const optional<uint> timeout();
  const string appName() const;
};