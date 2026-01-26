set(PROJECT_NAME server)

project(${PROJECT_NAME})

file(GLOB_RECURSE SOURCE_FILES 
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

foreach(SOURCE IN ITEMS ${SOURCE_FILES})
    get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    file(RELATIVE_PATH SOURCE_PATH_REL "${CMAKE_CURRENT_SOURCE_DIR}" "${SOURCE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
    source_group("${GROUP_PATH}" FILES "${SOURCE}")
endforeach()

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