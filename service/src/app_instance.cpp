#include "app_instance.hpp"

#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "generic/generic.hpp"

namespace fs = std::filesystem;

AppInstance::AppInstance(const fs::path& configPath, const ConnParams& cp) {
  configPath_ = configPath;
  readConfig();

  cp_ = make_unique<ConnParams>(cp);
  const string appName = configPath.stem().string();

  cout << "Creating configuration listener instance for " << appName << endl;
  cout << "Configuration path is '" << configPath.string() << "'" << endl;

  object_ =
    sdbus::createObject(cp.conn, sdbus::ObjectPath(cp.objectPath + appName));

  object_
    ->addVTable(
      sdbus::MethodVTableItem{sdbus::MethodName{"ChangeConfiguration"},
                              sdbus::Signature{"sv"},
                              {},
                              sdbus::Signature{"s"},
                              {},
                              setConfigCallback(),
                              {}},
      sdbus::MethodVTableItem{sdbus::MethodName{"GetConfiguration"},
                              sdbus::Signature{""},
                              {},
                              sdbus::Signature{"a{sv}"},
                              {},
                              getConfigCallback(),
                              {}},
      sdbus::SignalVTableItem{cp_->signalName, sdbus::Signature{"sv"}, {}, {}})
    .forInterface(cp.interfaceName);
}

void AppInstance::writeConfig() {
  ofstream ofs(configPath_);
  if (!ofs.is_open()) {
    throw std::runtime_error("can't write file " + configPath_.string());
  }

  Json::Value root;

  for (const auto& [key, value] : dict_) {
    if (value.containsValueOfType<string>()) {
      root[key] = value.get<string>();
    } else if (value.containsValueOfType<uint>()) {
      root[key] = value.get<uint>();
    } else {
      cerr << "unknown type: " << key << endl;
    }
  }
  ofs << root;

  ofs.close();
}

void AppInstance::readConfig() {
  const lock_guard<mutex> lock(mu_);
  generic::readConfig(dict_, configPath_);
}

sdbus::method_callback AppInstance::getConfigCallback() {
  return [this](const sdbus::MethodCall call) {
    thread([this, call = std::move(call)]() {
      auto reply = call.createReply();
      lock_guard<mutex> lock(this->mu_);
      reply << dict_;
      reply.send();
    }).detach();
  };
}

sdbus::method_callback AppInstance::setConfigCallback() {
  return [this](sdbus::MethodCall call) {
    thread([this, call = std::move(call)]() mutable {
      string key;
      sdbus::Variant value;
      call >> key >> value;
      auto reply = call.createReply();
    
      auto handleError = [&](const string& message) {
        cerr << message << endl;
        reply << message;
        reply.send();
      };
    
      lock_guard<mutex> lock(mu_);
      if (!isKeyExists(key)) {
        handleError("unknown key '" + key + "', discarded");
        return;
      }
    
      if (!isTypeMatches(key, value)) {
        auto errString = "invalid type of key '" + key + 
          "', expected '" + dict_[key].peekValueType() + "'";
        handleError(errString);
        return;
      }
    
      dict_[key] = value;
    
      if (!writeConfigSafely(handleError))
        return;
    
      if (!emitConfigChangedSignal(key, handleError))
        return;
    
      reply << "Key '" + key + "' was set";
      reply.send();
    }).detach();
  };
}

bool AppInstance::isKeyExists(const string& key) const {
  return dict_.find(key) != dict_.end();
}

bool AppInstance::isTypeMatches(const string& key, const sdbus::Variant& value) const {
  return strcmp(dict_.at(key).peekValueType(), value.peekValueType()) == 0;
}

bool AppInstance::writeConfigSafely(ErrFunc handleError) {
  try {
    writeConfig();
    return true;
  } catch (const fs::filesystem_error& e) {
    handleError("filesystem error: " + string(e.what()));
  } catch (const Json::Exception& e) {
    handleError("json error: " + string(e.what()));
  } catch (const std::exception& e) {
    handleError("unknown error: " + string(e.what()));
  }
  return false;
}

bool AppInstance::emitConfigChangedSignal(const string& key, ErrFunc handleError) {
  try {
    auto signal = object_->createSignal(cp_->interfaceName, cp_->signalName);
    signal << key << dict_[key];
    object_->emitSignal(signal);
    return true;
  } catch (const sdbus::Error& e) {
    handleError("sdbus error: " + string(e.what()));
  } catch (const std::exception& e) {
    handleError("unknown signal error: " + string(e.what()));
  }
  return false;
}
