#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

using namespace std;

/// @brief Класс для ведения логов в программе
class Logger {
 private:
  ostream& out_;
  mutex mu_;

  enum class Level { INFO, WARNING, ERROR };

 public:
  class LogStream {
   private:
    ostream& out_;
    mutex& mu_;
    Level level_;
    ostringstream buffer_;

    const char* levelToString(Level level) const;

   public:
    LogStream(ostream& out, mutex& mu, Level level);

    template <typename T>
    LogStream& operator<<(const T& value) {
      buffer_ << value;
      return *this;
    }

    // Поддержка endl и др.
    LogStream& operator<<(ostream& (*manip)(ostream&));

    ~LogStream();
  };

  Logger(ostream& out = cout);

  /// @return Поток вывода с меткой `Level::INFO`
  LogStream info();

  /// @return Поток вывода с меткой `Level::WARNING`
  LogStream warning();

  /// @return Поток вывода с меткой `Level::ERROR`
  LogStream error();
};

/// @brief Экспорт глобальной переменной `Logger`
extern Logger log;