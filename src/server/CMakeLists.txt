set(PROJECT_NAME server)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE core)

target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_SERVER)