cmake_minimum_required(VERSION 4.0.1)
project(confManagerApplication)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(sdbus-c++ REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(include)
file(GLOB SRCS
  "src/*.cpp"
)

add_executable(confManagerApplication ${SRCS})
target_link_libraries(confManagerApplication PRIVATE sdbus-c++ ${JSONCPP_LIBRARIES})