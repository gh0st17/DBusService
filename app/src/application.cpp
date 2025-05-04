#include "application.hpp"

#include "generic/generic.hpp"

ConfManagerApplication::ConfManagerApplication(const fs::path& configPath,
                                               condition_variable* cv)
    : configPath_(configPath), appName_(configPath.stem().string()), cv_(cv) {
  readConfig();
  generic::printConfig(dict_, appName_);

  conn_ = sdbus::createSessionBusConnection();
  proxy_ = sdbus::createProxy(*conn_, serviceName_,
                              sdbus::ObjectPath(objectPath_ + appName_));
  proxy_->registerSignalHandler(interfaceName_, signalName_, signalCallback());
}

sdbus::signal_handler ConfManagerApplication::signalCallback() {
  return [this](sdbus::Signal signal) {
    thread([this, signal = std::move(signal)]() mutable {
      string key;
      sdbus::Variant value;
      signal >> key >> value;

      logger_.info() << appName_ << ": recieved key: '" << key
                     << "' with value " << generic::stringValue(value);

      const lock_guard<mutex> lock(mu_);
      if (dict_.find(key) == dict_.end()) {
        logger_.error() << "unknown key '" << key << "', discarded";
        return;
      }

      dict_[key] = value;
      cv_->notify_all();
    }).detach();
  };
}

void ConfManagerApplication::readConfig() {
  const lock_guard<mutex> lock(mu_);
  generic::readConfig(dict_, configPath_);
}

void ConfManagerApplication::start() {
  logger_.info() << appName_ << ": Entering into event loop";
  conn_->enterEventLoopAsync();
}

void ConfManagerApplication::stop() {
  logger_.info() << appName_ << ": Leaving event loop";
  conn_->leaveEventLoop();
}

const bool ConfManagerApplication::waitTimeout(unique_lock<mutex>& lock) const {
  uint timeout = 10, oldTimeout;

  auto updateTimeout = [&]() {
    if (dict_.at("Timeout").containsValueOfType<uint>()) {
      timeout = dict_.at("Timeout").get<uint>();
    }
  };
  updateTimeout();

  oldTimeout = timeout;

  return cv_->wait_for(lock, chrono::seconds(timeout), [&] {
    updateTimeout();
    return oldTimeout != timeout || generic::stop.load();
  });
}

void ConfManagerApplication::printTimeoutPhrase() {
  const lock_guard<mutex> lock(mu_);

  if (dict_.find("TimeoutPhrase") == dict_.end()) {
    logger_.info() << appName_ << ": TimeoutPhrase: <key unset>";
  } else if (dict_["TimeoutPhrase"].containsValueOfType<string>()) {
    string value = dict_["TimeoutPhrase"].get<string>();
    logger_.info() << appName_ << ": TimeoutPhrase: '" << value << "'";
  } else {
    logger_.warning() << appName_ << ": TimeoutPhrase is not string type";
  }
}