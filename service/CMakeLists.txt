cmake_minimum_required(VERSION 3.10)
project(DBusService)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sdbus-c++ SHARED IMPORTED)
set_target_properties(sdbus-c++ PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/libsdbus-c++.so.2.1.0"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/include/sdbus-c++"
)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(include ${JSONCPP_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
file(GLOB SRCS
  "src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(DBusService ${SRCS})
target_link_libraries(DBusService PRIVATE sdbus-c++ ${JSONCPP_LIBRARIES} Threads::Threads)
