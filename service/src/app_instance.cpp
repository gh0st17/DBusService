#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include "app_instance.hpp"

#include <json/json.h>

namespace fs = std::filesystem;

AppInstance::AppInstance(const fs::path& configPath, const ConnParams& cp) {
  this->configPath = configPath;
  readConfig();
  
  this->cp = make_unique<ConnParams>(cp);
  const string appName = configPath.stem().string();

  cout << format("Creating configuration listener instance for {}\n", appName);
  cout << format("Configuration path is {}\n", configPath.string());

  object = sdbus::createObject(cp.conn, sdbus::ObjectPath(cp.objectPath + appName));
  signal = object->createSignal(cp.interfaceName, signalName);

  auto setConfigCallback = [this](const string& key, const sdbus::Variant& value) {
    this->setConfigCallback(key, value);
  };

  object->addVTable(
    sdbus::registerMethod(sdbus::MethodName("ChangeConfiguration"))
    .implementedAs(setConfigCallback)
  ).forInterface(sdbus::InterfaceName(cp.interfaceName));

  auto getConfigCallback = [this]() {
    return this->getConfigCallback();
  };

  object->addVTable(
    sdbus::registerMethod(sdbus::MethodName("GetConfiguration"))
    .implementedAs(getConfigCallback)
  ).forInterface(cp.interfaceName);

  object->addVTable(
    sdbus::registerSignal(sdbus::SignalName(signalName))
  ).forInterface(cp.interfaceName);
}

void AppInstance::writeConfig() const {
  ofstream ofs(configPath);
  if (!ofs.is_open()) {
    throw std::runtime_error("Can't write file " + configPath.string());
  }

  Json::Value root;

  for (const auto& [key, value] : dict) {
    if (value.containsValueOfType<string>()) {
      root[key] = value.get<string>();
    } else if (value.containsValueOfType<uint>()) {
      root[key] = value.get<uint>();
    } else if (value.containsValueOfType<int>()) {
      root[key] = value.get<int>();
    } else if (value.containsValueOfType<bool>()) {
      root[key] = value.get<bool>();
    } else {
      cerr << "Unknown type: " << key << endl;
    }
  }
  ofs << root;

  ofs.close();
}

void AppInstance::readConfig(){
  ifstream ifs(configPath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Can't open file: " + configPath.string());
  }

  Json::Value root;
  ifs >> root;

  for (const auto& key : root.getMemberNames()) {
    const Json::Value& val = root[key];

    cout << format("{}: {}\n", key, val.asString());

    if (val.isUInt()) {
      dict[key] = sdbus::Variant(static_cast<uint>(val.asUInt()));
    } else if (val.isInt()) {
      dict[key] = sdbus::Variant(static_cast<int>(val.asInt()));
    } else if (val.isString()) {
      dict[key] = sdbus::Variant(val.asString());
    } else if (val.isBool()) {
      dict[key] = sdbus::Variant(val.asBool());
    } else {
      cerr << "Unknown type: " << key << std::endl;
    }
  }

  ifs.close();  
}

config AppInstance::getConfigCallback() const {
  return dict;
}

void AppInstance::setConfigCallback(const string& key, const sdbus::Variant& value) {
  dict[key] = value;
  try {
    writeConfig();
  }
  catch (const fs::filesystem_error& e) {
    cerr << format(
      "filesystem error while writing conf: {}\n",
      e.what()
    );

    return;
  }
  catch (const Json::Exception& e) {
    cerr << format(
      "json error while writing conf: {}\n",
      e.what()
    );

    return;
  }
  catch (const std::exception& e) {
    cerr << format(
      "unknown error while writing conf: {}\n",
      e.what()
    );

    return;
  }

  object->emitSignal(signal);
}