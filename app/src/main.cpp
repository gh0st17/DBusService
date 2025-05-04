#include <csignal>
#include <iostream>
#include <list>
#include <memory>
#include <thread>

#include "application.hpp"
#include "generic/generic.hpp"
#include "generic/params.hpp"

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
  condition_variable cv;
  vector<thread> threads;
  threads.reserve(configsPaths.size());

  list<shared_ptr<ConfManagerApplication> > apps;
  for (const auto& path : configsPaths) {
    auto app = make_shared<ConfManagerApplication>(path, &cv);
    apps.push_back(app);
    apps.back()->start();

    threads.push_back(thread([app, &mu]() mutable {
      unique_lock<mutex> lock(mu);
      while (!generic::stop.load()) {
        if (!app->waitTimeout(lock)) {
          app->printTimeoutPhrase();
        }
      }
    }));
  }

  thread([&]() {
    while (!generic::stop.load()) {
      this_thread::sleep_for(chrono::seconds(1));
    }
    for_each(apps.begin(), apps.end(), mem_fn(&ConfManagerApplication::stop));
    cv.notify_all();
  }).detach();

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}