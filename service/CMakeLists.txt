cmake_minimum_required(VERSION 4.0.1)
project(DBusService)

include_directories(include)
file(GLOB SRCS
  "src/*.cpp"
)

add_executable(DBusService ${SRCS})
target_link_libraries(DBusService PRIVATE sdbus-c++ ${JSONCPP_LIBRARIES})