#include <csignal>
#include <iostream>
#include <thread>

#include "generic/generic.hpp"
#include "generic/params.hpp"
#include "service.hpp"

using namespace std;

int main(const int argc, const char* argv[]) {
  return !generic::errorHandler([=]() {
    signal(SIGINT, generic::signalHandler);
    parse_params(argc, argv);

    auto service = make_shared<DBusService>(std::move(p.configsPaths));

    thread stopThread([&]() {
      while (!generic::stop.load()) {
        this_thread::sleep_for(chrono::seconds(1));
      }
      service->stop();
    });

    thread startThread([&]() { service->start(); });

    startThread.join();
    stopThread.join();
  });
}