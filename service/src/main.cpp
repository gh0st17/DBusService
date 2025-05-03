#include <iostream>
#include <thread>
#include <csignal>

#include "generic/params.hpp"
#include "generic/generic.hpp"
#include "service.hpp"

using namespace std;

int main(const int argc, const char* argv[]) {
  try {
    signal(SIGINT, generic::signalHandler);
    parse_params(argc, argv);

    auto service = make_shared<DBusService>(std::move(p.configsPaths));

    thread stopThread([&]() {
        while (!generic::stop.load()) {
            this_thread::sleep_for(chrono::seconds(1));
        }
        service->stop();
    });

    thread startThread([&]() {
        service->start();
    });

    startThread.join();
    stopThread.join();

  } catch (const fs::filesystem_error& e) {
    cerr << "filesystem error: " << e.what() << endl;
  } catch (const std::bad_cast& e) {
    cerr << "bad_cast error: " << e.what() << endl;
  } catch (const std::bad_alloc& e) {
    cerr << "bad_alloc error: " << e.what() << endl;
  } catch (const Json::Exception& e) {
    cerr << "json error: " << e.what() << endl;
  } catch (const sdbus::Error& e) {
    cerr << "sdbus error: " << e.what() << endl;
  } catch (const std::invalid_argument& e) {
    cerr << "invalid_argument error: " << e.what() << endl;
  } catch (const std::exception& e) {
    cerr << "unknown error: " << e.what() << endl;
  }
}