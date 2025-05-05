#include "app_instance.hpp"

#include <json/json.h>

#include <fstream>
#include <sstream>

#include "generic/generic.hpp"

AppInstance::AppInstance(const fs::path& configPath,
                         const shared_ptr<ConnParams> cp)
    : cp_(cp), configPath_(configPath), appName_(configPath.stem().string()) {
  logger_.info() << "Read config '" << configPath.string() << "'";
  readConfig();

  logger_.info() << "Creating configuration listener instance for " << appName_;
  logger_.info() << "Configuration path is '" << configPath.string() << "'";
  generic::printConfig(dict_, appName_);

  object_ =
    sdbus::createObject(cp->conn, sdbus::ObjectPath(cp->objectPath + appName_));

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
    .forInterface(cp->interfaceName);
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
      logger_.error() << "unknown type: " << key;
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
      generic::errorHandler([&]() {
        auto reply = call.createReply();
        lock_guard<mutex> lock(this->mu_);
        reply << dict_;
        reply.send();
      });
    }).detach();
  };
}

sdbus::method_callback AppInstance::setConfigCallback() {
  return [this](sdbus::MethodCall call) {
    thread([this, call = std::move(call)]() {
      setConfigHandler(std::move(call));
    }).detach();
  };
}

void AppInstance::setConfigHandler(sdbus::MethodCall call) {
  generic::errorHandler([&]() mutable {
    string key;
    sdbus::Variant value;
    call >> key >> value;

    auto handleError = [&](const string& message) {
      const auto errName =
        sdbus::Error::Name(cp_->interfaceName + "." + call.getMemberName());
      logger_.error() << appName_ << ": " << message;
      auto reply = call.createErrorReply({errName, message});
      reply.send();
    };

    lock_guard<mutex> lock(mu_);
    if (dict_.find(key) == dict_.end()) {  // Проверяем наличие ключа `key`
      string message = "unknown key '" + key + "', discarded";
      handleError(message);
      return;
    }

    if (!isTypeMatches(key, value)) {
      string message = "invalid type of key '" + key + "', expected '" +
                       dict_[key].peekValueType() + "'";
      handleError(message);
      return;
    }

    dict_[key] = value;
    writeConfig();

    // Излучаем сигнал об изменении конфигурации для соответствующего приложения
    auto signal = object_->createSignal(cp_->interfaceName, cp_->signalName);
    signal << key << dict_[key];
    object_->emitSignal(signal);

    stringstream ss;
    ss << appName_ << ": Key '" << key << "' was set to "
       << generic::stringValue(value);
    replyAnswer(call, ss.str());
  });
}

const bool AppInstance::isTypeMatches(const string& key,
                                      const sdbus::Variant& value) const {
  return strcmp(dict_.at(key).peekValueType(), value.peekValueType()) == 0;
}

void AppInstance::replyAnswer(sdbus::MethodCall& call, const string& message) {
  auto reply = call.createReply();
  reply << message;
  reply.send();
  logger_.info() << message;
}