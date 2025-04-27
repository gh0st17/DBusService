cmake_minimum_required(VERSION 3.20)
project(DBusService)

add_subdirectory(service)
add_subdirectory(app)

set_target_properties(DBusService PROPERTIES
  BUILD_RPATH "$ORIGIN" # Для поиска библиотеки в директории сборки укажем эту директрию
)