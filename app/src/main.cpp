#include <iostream>
#include <thread>
#include <list>
#include <memory>

#include "app_params.hpp"
#include "application.hpp"

using namespace std;

void handleApplications(const vector<fs::path>&);

int main(const int argc, const char *argv[]) {
  try {
    parse_params(argc, argv);

    handleApplications(p.configsPaths);
  }
  catch (const fs::filesystem_error& e) {
    cerr << "filesystem error: " << e.what() << endl;
  }
  catch (const std::bad_cast& e) {
    cerr << "bad_cast error: " << e.what() << endl;
  }
  catch (const std::bad_alloc& e) {
    cerr << "bad_alloc error: " << e.what() << endl;
  }
  catch(const Json::Exception& e){
    cerr << "json error: " << e.what() << endl;
  }
  catch (const sdbus::Error& e) {
    cerr << "sdbus error: " << e.what() << endl;
  }
  catch (const std::exception& e) {
    cerr << "unknown error: " << e.what() << endl;
  }
}

void handleApplications(const vector<fs::path>& configsPaths) {
  list<unique_ptr<ConfManagerApplication>> apps;
  vector<thread> threads;
  mutex mu;
  threads.reserve(configsPaths.size());

  for (const auto& path : configsPaths) {
    auto app = make_unique<ConfManagerApplication>(path);
    apps.push_back(std::move(app));
    apps.back()->start();

    threads.push_back(thread([cma = apps.back().get(), &mu] () mutable {
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
      }
    }));
  }

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
