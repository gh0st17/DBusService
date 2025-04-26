#include <iostream>
#include <fstream>
#include <format>

#include "application.hpp"

ConfManagerApplication::ConfManagerApplication(const fs::path& configPath) {
  this->configPath = configPath;
  readConfig();
  const string appName = configPath.stem().string();

  conn = sdbus::createSessionBusConnection();

  proxy = sdbus::createProxy(*conn, serviceName, sdbus::ObjectPath(objectPath + appName));
  proxy->uponSignal(signalName)
    .onInterface(interfaceName)
    .call([this]() { this->readConfig(); });
}

void ConfManagerApplication::readConfig(){
  ifstream ifs(configPath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Can't open file: " + configPath.string());
  }

  Json::Value root;
  ifs >> root;

  const lock_guard<mutex> lock(mu);
  for (const auto& key : root.getMemberNames()) {
    const Json::Value& val = root[key];

    cout << format("{}: {}\n", key, val.asString());

    if (val.isUInt()) {
      dict[key] = static_cast<uint>(val.asUInt());
    } else if (val.isInt()) {
      dict[key] = static_cast<int>(val.asInt());
    } else if (val.isString()) {
      dict[key] = val.asString();
    } else if (val.isBool()) {
      dict[key] = val.asBool();
    } else {
      std::cerr << "Unknown type: " << key << std::endl;
    }
  }

  ifs.close();  
}

void ConfManagerApplication::start() {
  conn->enterEventLoopAsync();
}

void ConfManagerApplication::printTimeoutPhrase() {
  const lock_guard<mutex> lock(mu);

  if (dict.find("TimeoutPhrase") == dict.end()) {
    cout << "TimeoutPhrase: <Key unset>\n";
  }
  else if (dict["TimeoutPhrase"].type() == typeid(string)) {
    string value = any_cast<string>(dict["TimeoutPhrase"]);
    cout << format("TimeoutPhrase: {}\n", value);
  }
}


const optional<uint> ConfManagerApplication::timeout() {
  const lock_guard<mutex> lock(mu);

  if (dict.find("Timeout") == dict.end()) {
    cout << "<Timeout unset>\n";
  }
  else if (dict["Timeout"].type() == typeid(uint)) {
    uint value = any_cast<uint>(dict["Timeout"]);
    return value;
  }
  else {
    cout << "Timeout is not integer type\n";
  }

  return {};
}

const string ConfManagerApplication::appName() const {
  return configPath.stem().string();
}