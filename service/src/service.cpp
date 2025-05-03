#include "service.hpp"

#include <filesystem>
#include <fstream>

#include "generic/logger.hpp"

namespace fs = std::filesystem;

DBusService::DBusService(vector<fs::path>&& configsPaths) {
  conn_ = sdbus::createSessionBusConnection();

  conn_->requestName(sdbus::ServiceName(serviceName_));

  for (const auto& path : configsPaths) {
    initInstance(path);
  }
}

void DBusService::initInstance(const fs::path& configPath) {
  const static shared_ptr<ConnParams> cp = make_shared<ConnParams>(
    ConnParams{.conn = *(conn_),
               .interfaceName = interfaceName_,
               .signalName = signalName_,
               .objectPath = sdbus::ObjectPath(objectPath_ + "Application/")});

  auto instance = make_unique<AppInstance>(configPath, cp);

  instances_.push_back(std::move(instance));
}

void DBusService::start() {
  Logger::getInstance().info() << "Entering into event loop";
  conn_->enterEventLoop();
}

void DBusService::stop() {
  Logger::getInstance().info() << "Leaving event loop";
  instances_.clear();
  conn_->leaveEventLoop();
}