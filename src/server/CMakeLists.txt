set(PROJECT_NAME server)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
	main.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${enet_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        enet
        core
        spdlog::spdlog
        glm
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            ws2_32
            winmm
    )
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_SERVER)