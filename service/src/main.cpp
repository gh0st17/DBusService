#include <iostream>
#include <format>

#include "params.hpp"
#include "service.hpp"

using namespace std;

int main(const int argc, const char *argv[]) {
  try {
    parse_params(argc, argv);
    
    DBusService service(std::move(p.configsPaths));

    // service.startService();
  }
  catch (const fs::filesystem_error& e) {
    cerr << format("Filesystem error: {}\n", e.what());
  }
  catch (const std::bad_cast& e) {
    cerr << format("Cast error: {}\n", e.what());
  }
  catch (const std::bad_alloc& e) {
    cerr << format("bad_alloc error: {}\n", e.what());
  }
  catch (const sdbus::Error& e) {
    cerr << format("sdbus error: {}\n", e.what());
  }
  catch (const std::exception& e) {
    cerr << format("Unknown error: {}\n", e.what());
  }
}