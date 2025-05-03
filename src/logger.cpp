#include "generic/logger.hpp"

#include <iostream>
#include <mutex>
#include <sstream>

Logger::Logger(ostream& out) : out_(out) {
}

Logger::LogStream::LogStream(ostream& out, mutex& mu, Level level)
    : out_(out), mu_(mu), level_(level) {
}

// Поддержка endl и др.
Logger::LogStream& Logger::LogStream::operator<<(ostream& (*manip)(ostream&)) {
  buffer_ << manip;
  return *this;
}

Logger::LogStream::~LogStream() {
  lock_guard<mutex> lock(mu_);
  out_ << "[" << levelToString(level_) << "] " << buffer_.str() << endl;
}

const char* Logger::LogStream::levelToString(Level level) const {
  switch (level) {
    case Level::INFO:
      return "  INFO ";
    case Level::WARNING:
      return "WARNING";
    case Level::ERROR:
      return " ERROR ";
  }
  return "UNKNOWN";
}

Logger& Logger::getInstance(ostream& out) {
  static Logger instance(out);
  return instance;
}

Logger::LogStream Logger::info() {
  return LogStream(out_, mu_, Level::INFO);
}

Logger::LogStream Logger::warning() {
  return LogStream(out_, mu_, Level::WARNING);
}

Logger::LogStream Logger::error() {
  return LogStream(out_, mu_, Level::ERROR);
}