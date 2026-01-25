set(PROJECT_NAME core)
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
	test.cpp
)

target_sources(${PROJECT_NAME}
	PUBLIC
	include/core/time.hpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include)
		
target_compile_definitions(${PROJECT_NAME} PUBLIC GAME_CORE)