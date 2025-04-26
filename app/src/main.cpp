#include <iostream>
#include <format>

#include "app_params.hpp"
#include "application.hpp"

using namespace std;

int main(const int argc, const char *argv[]) {
  try {
    parse_params(argc, argv);
    
    // ConfManagerApplication confManagerApplication1(p.configPath);

    // confManagerApplication1.start();
  }
  catch (const fs::filesystem_error& e) {
    cerr << format("filesystem error: {}\n", e.what());
  }
  catch (const std::bad_cast& e) {
    cerr << format("bad_cast error: {}\n", e.what());
  }
  catch (const std::bad_alloc& e) {
    cerr << format("bad_alloc error: {}\n", e.what());
  }
  catch(const Json::Exception& e){
    cerr << format("json error: {}\n", e.what());
  }
  catch (const sdbus::Error& e) {
    cerr << format("sdbus error: {}\n", e.what());
  }
  catch (const std::exception& e) {
    cerr << format("unknown error: {}\n", e.what());
  }
}