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
	src/application/application.h
	src/application/application.cpp
	src/auth/tokens.h
	src/auth/tokens.cpp
	src/file_system/file_system.cpp
	src/file_system/file_system.h
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/json/json_handler.h
	src/json/json_handler.cpp
	src/json/boost_json.cpp
	src/logger/logger.h
	src/logger/logger.cpp
	src/logger/logging_handler.cpp
	src/logger/logging_handler.h
	src/model/constdef.h
	src/model/game.h
	src/model/game.cpp
	src/model/game_session.h
	src/model/game_session.cpp
	src/model/god.h
	src/model/god.cpp
	src/model/map.cpp
	src/model/map.h
	src/model/player.h
	src/model/player.cpp
	src/model/road.h
	src/model/road.cpp
	src/model/road_layout.h
	src/model/road_layout.cpp
	src/model/typedef.h
	src/model/typedef.cpp
	src/other/sdk.h
	src/other/tagged.h
	src/other/utils.h
	src/other/utils.cpp
	src/request/api_request_handler.h
	src/request/api_request_handler_proxy.h
	src/request/static_file_request_handler.h
	src/request/static_file_request_handler_proxy.h
	src/request/request_handler.h
	src/request/request_handler_unit.h
	src/request/request_handler.cpp
	src/server/http_server.cpp
	src/server/http_server.h


)
target_include_directories(game_server PRIVATE
	src
	src/application
	src/auth
	src/file_system
	src/json
	src/logger
	src/model
	src/other
	src/request
	src/server


)
target_link_libraries(game_server PRIVATE Threads::Threads Boost::log Boost::log_setup)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)