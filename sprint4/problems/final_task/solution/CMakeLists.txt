cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.85.0 REQUIRED)
find_package(Boost 1.85.0 COMPONENTS log log_setup program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_library(UserStaticLib STATIC
	src/ModelLayer/collision/geom.h
	src/ModelLayer/collision/collision_detector.cpp
	src/ModelLayer/collision/collision_detector.h
	src/ModelLayer/type_const_defines/constdef.h
	src/ModelLayer/model/dog.h
	src/ModelLayer/model/dog.cpp
	src/ModelLayer/gather/dog_gather.h
	src/ModelLayer/gather/dog_gather.cpp
	src/ModelLayer/model/game.h
	src/ModelLayer/model/game.cpp
	src/ModelLayer/model/map.cpp
	src/ModelLayer/model/map.h
	src/ModelLayer/model/road.h
	src/ModelLayer/model/road.cpp
	src/ModelLayer/model/road_layout.h
	src/ModelLayer/model/road_layout.cpp
	src/ModelLayer/type_const_defines/typedef.h
	src/ModelLayer/type_const_defines/typedef.cpp
	src/ModelLayer/loot_implement/loot_generator.h
	src/ModelLayer/loot_implement/loot_generator.cpp
	src/ModelLayer/loot_implement/lost_obj.h
	src/ModelLayer/loot_implement/lost_obj.cpp
	src/InterLayer/other/sdk.h
	src/InterLayer/other/tagged.h
	src/InterLayer/other/utils.h
	src/InterLayer/other/utils.cpp
	src/InterLayer/json/tag_invoke_handler.cpp
	src/InterLayer/json/tag_invoke_handler.h
)

target_include_directories(UserStaticLib PUBLIC 
	${ZLIB_INCLUDES}
	src/ModelLayer/collision
	src/ModelLayer/gather
	src/ModelLayer/loot_implement
	src/ModelLayer/model
	src/ModelLayer/type_const_defines
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
	src/ApplicationLayer/application/player_data_record.h
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
	src/InterLayer/serialization/lo_serialize.h
	src/InterLayer/serialization/lo_serialize.cpp
	src/InterLayer/serialization/player_serialize.h
	src/InterLayer/serialization/player_serialize.cpp
	src/InterLayer/serialization/game_session_serialize.h
	src/InterLayer/serialization/game_session_serialize.cpp
	src/InterLayer/serialization/dog_serialize.h
	src/InterLayer/serialization/dog_serialize.cpp
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
	src/InterLayer/postgres/use_cases_impl.h
	src/InterLayer/postgres/use_cases_impl.cpp
	src/InterLayer/postgres/use_cases.h
	src/InterLayer/postgres/postgres.cpp
	src/InterLayer/postgres/postgres.h
	src/InterLayer/postgres/connection_pool.h
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
	src/InterLayer/serialization
	src/InterLayer/postgres
	src/ServerLayer/request
	src/ServerLayer/server

)
target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost CONAN_PKG::libpq CONAN_PKG::libpqxx UserStaticLib)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_executable(game_server_tests
	tests/loot_generator_tests.cpp
)

target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2 UserStaticLib)
catch_discover_tests(game_server_tests)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)