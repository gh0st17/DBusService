cmake_minimum_required(VERSION 3.10)
project(DBusConfigManager)

find_package(sdbus-c++ REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(include service/include)
set(SOURCE_EXE service/main.cpp)
file(GLOB SRCS
  "service/*.cpp"
)

add_executable(config-service ${SOURCE_EXE} ${SRCS})
target_link_libraries(config-service PRIVATE sdbus-c++ ${JSONCPP_LIBRARIES})

add_subdirectory(service)
add_subdirectory(app)