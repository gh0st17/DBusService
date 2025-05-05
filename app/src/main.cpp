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
  return !generic::errorHandler([=]() {
    signal(SIGINT, generic::signalHandler);
    parse_params(argc, argv);  // Обрабатываем входные параметры программы

    handleApplications(p.configsPaths);
  });
}

void handleApplications(const vector<fs::path>& configsPaths) {
  mutex mu;
  condition_variable cv;
  vector<thread> threads;
  threads.reserve(configsPaths.size());

  list<shared_ptr<ConfManagerApplication> > apps;
  for (const auto& path : configsPaths) {  // Для каждого файла конфигурации
                                           // создаем экземпляр приложения,
                                           // кладем его в список и запускаем
                                           // в отдельном потоке
    auto app = make_shared<ConfManagerApplication>(path, &cv);
    apps.push_back(app);

    threads.push_back(thread([app, &mu]() mutable {
      app->start();
      unique_lock<mutex> lock(mu);
      while (!generic::stop.load()) {  // Пока сигнал SIGINT не получен
        if (!app->waitTimeout(lock)) {
          app->printTimeoutPhrase();
        }
      }
    }));
  }

  thread([&]() {  // Поток для отслеживания что программы пора завершить
    while (!generic::stop.load()) {
      this_thread::sleep_for(chrono::seconds(1));
    }
    for_each(apps.begin(), apps.end(), mem_fn(&ConfManagerApplication::stop));
    cv.notify_all();
  })
    .detach();

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}