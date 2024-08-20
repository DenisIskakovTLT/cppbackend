cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
find_package(Boost 1.78.0 COMPONENTS log log_setup)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/file_system/file_system.cpp
	src/file_system/file_system.h
	src/server/http_server.cpp
	src/server/http_server.h
	src/other/sdk.h
	src/model/model.h
	src/model/model.cpp
	src/other/tagged.h
	src/json/boost_json.cpp
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/request/request_handler.cpp
	src/request/request_handler.h
	src/json/json_handler.cpp
	src/json/json_handler.h
	src/responce/responce_handler.cpp
	src/responce/responce_handler.h
	src/responce/static_file_handler.cpp
	src/responce/static_file_handler.h
	src/logger/logger.h
	src/logger/logger.cpp
	src/logger/logging_handler.h
	src/logger/logging_handler.cpp
)

target_include_directories(game_server PRIVATE
	src
	src/file_system
	src/json
	src/logger
	src/model
	src/other
	src/request
	src/responce
	src/server


)
target_link_libraries(game_server PRIVATE Threads::Threads Boost::log Boost::log_setup)
# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)