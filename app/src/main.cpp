#include <iostream>
#include <list>
#include <memory>
#include <thread>
#include <csignal>

#include "application.hpp"
#include "generic/params.hpp"
#include "generic/generic.hpp"

using namespace std;

/// @brief Обработчик приложении
/// @param configsPaths Массив путей к конфигурациям приложении
void handleApplications(const vector<fs::path>&);

int main(const int argc, const char* argv[]) {
  try {
    signal(SIGINT, generic::signalHandler);
    parse_params(argc, argv);

    handleApplications(p.configsPaths);
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

void handleApplications(const vector<fs::path>& configsPaths) {
  mutex mu;
  vector<thread> threads;
  threads.reserve(configsPaths.size());

  list<unique_ptr<ConfManagerApplication>> apps;
  for (const auto& path : configsPaths) {
    auto app = make_unique<ConfManagerApplication>(path);
    apps.push_back(std::move(app));
    apps.back()->start();

    threads.push_back(thread([cma = apps.back().get(), &mu]() mutable {
      uint timeout = 10;
      while (true) {
        auto timeoutOpt = cma->timeout();
        if (timeoutOpt.has_value()) {
          timeout = timeoutOpt.value();
        }
        this_thread::sleep_for(chrono::seconds(timeout));
        {
          const lock_guard<mutex> lock(mu);
          cma->printTimeoutPhrase();
        }
        if (generic::stop.load()) {
          cma->stop();
          break;
        }
      }
    }));
  }

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}