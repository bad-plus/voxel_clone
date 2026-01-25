set(PROJECT_NAME core)
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
    network/enet_context.cpp
)

target_sources(${PROJECT_NAME}
    PUBLIC
        include/core/time.hpp
        include/core/logger.hpp
        include/core/network/enet_context.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${enet_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        enet
        glm
        spdlog::spdlog
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            ws2_32
            winmm
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        GAME_CORE
)
