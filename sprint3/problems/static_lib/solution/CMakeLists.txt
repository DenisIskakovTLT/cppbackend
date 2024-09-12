cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
find_package(Boost 1.78.0 COMPONENTS log log_setup program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_library(UserStaticLib STATIC
	src/ModelLayer/model/constdef.h
	src/ModelLayer/model/dog.h
	src/ModelLayer/model/dog.cpp
	src/ModelLayer/model/game.h
	src/ModelLayer/model/game.cpp
	src/ModelLayer/model/map.cpp
	src/ModelLayer/model/map.h
	src/ModelLayer/model/road.h
	src/ModelLayer/model/road.cpp
	src/ModelLayer/model/road_layout.h
	src/ModelLayer/model/road_layout.cpp
	src/ModelLayer/model/typedef.h
	src/ModelLayer/model/typedef.cpp
	src/ModelLayer/model/loot_generator.h
	src/ModelLayer/model/loot_generator.cpp
	src/ModelLayer/model/lost_obj.h
	src/ModelLayer/model/lost_obj.cpp
	src/InterLayer/other/sdk.h
	src/InterLayer/other/tagged.h
	src/InterLayer/other/utils.h
	src/InterLayer/other/utils.cpp
	src/InterLayer/json/tag_invoke_handler.cpp
	src/InterLayer/json/tag_invoke_handler.h
)

target_include_directories(UserStaticLib PUBLIC 
	${ZLIB_INCLUDES}
	src/ModelLayer/model
	src/InterLayer/other
	src/InterLayer/json
)
	
add_executable(game_server
	src/main.cpp
	src/ApplicationLayer/application/application.h
	src/ApplicationLayer/application/application.cpp
	src/ApplicationLayer/application/game_session.h
	src/ApplicationLayer/application/game_session.cpp
	src/ApplicationLayer/application/player.h
	src/ApplicationLayer/application/player.cpp
	src/ApplicationLayer/auth/tokens.h
	src/ApplicationLayer/auth/tokens.cpp
	src/InterLayer/command/command.h
	src/InterLayer/command/command.cpp
	src/InterLayer/error/error_handler.h
	src/InterLayer/error/error_handler.cpp
	src/InterLayer/file_system/file_system.cpp
	src/InterLayer/file_system/file_system.h
	src/InterLayer/json/json_loader.h
	src/InterLayer/json/json_loader.cpp
	src/InterLayer/json/json_handler.h
	src/InterLayer/json/json_handler.cpp
	src/InterLayer/json/boost_json.cpp
	src/InterLayer/logger/logger.h
	src/InterLayer/logger/logger.cpp
	src/InterLayer/logger/logging_handler.cpp
	src/InterLayer/logger/logging_handler.h
	src/ServerLayer/request/api_request_handler.h
	src/ServerLayer/request/api_request_handler_proxy.h
	src/ServerLayer/request/static_file_request_handler.h
	src/ServerLayer/request/static_file_request_handler_proxy.h
	src/ServerLayer/request/request_handler.h
	src/ServerLayer/request/request_handler_unit.h
	src/ServerLayer/request/request_handler.cpp
	src/ServerLayer/server/http_server.cpp
	src/ServerLayer/server/http_server.h
	src/InterLayer/ticker/ticker.h
	src/InterLayer/ticker/ticker.cpp
)

target_include_directories(game_server PRIVATE
	src
	src/ApplicationLayer/application
	src/ApplicationLayer/auth
	src/InterLayer/command
	src/InterLayer/error
	src/InterLayer/file_system
	src/InterLayer/json
	src/InterLayer/logger
	src/InterLayer/ticker
	src/ServerLayer/request
	src/ServerLayer/server

)
target_link_libraries(game_server PRIVATE Threads::Threads Boost::log Boost::log_setup Boost::program_options UserStaticLib)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_executable(game_server_tests
	tests/loot_generator_tests.cpp
)

target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2 UserStaticLib)
catch_discover_tests(game_server_tests)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)