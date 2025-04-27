#include <iostream>

#include "params.hpp"
#include "service.hpp"

using namespace std;

int main(const int argc, const char* argv[]) {
  try {
    parse_params(argc, argv);

    DBusService service(std::move(p.configsPaths));

    service.startService();
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
  } catch (const std::exception& e) {
    cerr << "unknown error: " << e.what() << endl;
  }
}