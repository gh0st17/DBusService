#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <json/json.h>
#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

namespace generic {
  void readConfig(map<string, sdbus::Variant>&, const fs::path&);
}