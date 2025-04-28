#pragma once
#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

namespace generic {
void readConfig(map<string, sdbus::Variant>&, const fs::path&);
}