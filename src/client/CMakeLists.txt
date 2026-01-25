set(PROJECT_NAME client)
project(${PROJECT_NAME})

if(TARGET update_mappings)
    set_target_properties(update_mappings PROPERTIES FOLDER "External")
endif()
if(TARGET uninstall)
    set_target_properties(uninstall PROPERTIES FOLDER "External")
endif()
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

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE 
	${stb_SOURCE_DIR}
	${enet_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    glm
	freetype
    spdlog::spdlog
    enet
	core
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_CLIENT)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources 
    ${OUTPUT_DIR}/resources
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    GAME_CLIENT
    RESOURCES_PATH="${CMAKE_SOURCE_DIR}/resources/"
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
)