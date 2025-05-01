#include "service.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

DBusService::DBusService(vector<fs::path>&& configsPaths) {
  conn_ = sdbus::createSessionBusConnection();

  conn_->requestName(sdbus::ServiceName(serviceName_));

  for (const auto& path : configsPaths) {
    cout << "Read config '" << path.string() << "'" << endl;

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

void DBusService::startService() {
  cout << "Entering into event loop\n";
  conn_->enterEventLoop();
}