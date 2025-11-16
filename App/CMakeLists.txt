# App

set(SOURCES
    Source/Main.cpp
    Source/AppLayer.h
    Source/AppLayer.cpp
    Source/HUDLayer.h
    Source/HUDLayer.cpp
    Source/Camera.h
    Source/Camera.cpp
    Source/Block.h
    Source/Block.cpp
    Source/BlockRegistry.h
    Source/BlockRegistry.cpp
    Source/Chunk.h
    Source/Chunk.cpp
    Source/Perlin.h
    Source/Perlin.cpp
)

add_executable(App)

target_sources(App PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

target_link_libraries(App glfw)
target_link_libraries(App glad)
target_link_libraries(App glm)
target_link_libraries(App freetype)

target_link_libraries(App Core)

target_include_directories(App PRIVATE Source)

# Set working directory for VS debugger
set_target_properties(App PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/App"
)