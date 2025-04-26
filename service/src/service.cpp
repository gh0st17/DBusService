#include <iostream>
#include <fstream>
#include <filesystem>
#include "service.hpp"

using namespace filesystem;


DBusService::DBusService(vector<string>&& configsPaths) {
  conn = sdbus::createSessionBusConnection();

  conn->requestName(sdbus::ServiceName(serviceName));

  for (const auto& path : configsPaths) {
    cout << format("Read config '{}'\n", path);


    initObject(path);
  }
}

void DBusService::initObject(const string& configPath) {

}

void DBusService::startService() {
  conn->enterEventLoop();
}

void DBusService::writeConfig(const string& configPath) const {
  ofstream ofs(configPath);
  if (!ofs.is_open()) {
    throw std::runtime_error("Can't write file " + configPath);
  }

  Json::Value root;

  for (const auto& [key, value] : dict.at(configPath)) {
    if (value.containsValueOfType<string>()) {
      root[key] = value.get<string>();
    } else if (value.containsValueOfType<uint32_t>()) {
      root[key] = value.get<uint32_t>();
    } else if (value.containsValueOfType<int32_t>()) {
      root[key] = value.get<int32_t>();
    } else if (value.containsValueOfType<bool>()) {
      root[key] = value.get<bool>();
    } else {
      cerr << "Unknown type: " << key << endl;
    }
  }
  ofs << root;

  ofs.close();
}

void DBusService::readConfig(const string& configPath){
  ifstream ifs(configPath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Can't open file: " + configPath);
  }

  Json::Value root;
  ifs >> root;

  for (const auto& key : root.getMemberNames()) {
    const Json::Value& val = root[key];

    std::cout << key << ": " << val << std::endl;

    if (val.isUInt()) {
      dict[configPath][key] = sdbus::Variant(static_cast<uint32_t>(val.asUInt()));
    } else if (val.isInt()) {
      dict[configPath][key] = sdbus::Variant(static_cast<int32_t>(val.asInt()));
    } else if (val.isString()) {
      dict[configPath][key] = sdbus::Variant(val.asString());
    } else if (val.isBool()) {
      dict[configPath][key] = sdbus::Variant(val.asBool());
    } else {
      std::cerr << "Unknown type: " << key << std::endl;
    }
  }

  ifs.close();  
}