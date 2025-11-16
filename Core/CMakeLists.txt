# Core

set(SOURCES
    Vendor/stb/stb_image.h
    Vendor/stb/stb_image.cpp
    Source/Core/Application.h
    Source/Core/Application.cpp
    Source/Core/Window.h
    Source/Core/Window.cpp
    Source/Core/Layer.h
    Source/Core/Event.h
    Source/Core/Event.cpp
    Source/Core/Renderer/Texture.h
    Source/Core/Renderer/Texture.cpp
    Source/Core/Renderer/TextureAtlas.h
    Source/Core/Renderer/TextureAtlas.cpp
    Source/Core/Renderer/Shader.h
    Source/Core/Renderer/Shader.cpp
    Source/Core/Renderer/Mesh.h
    Source/Core/Renderer/Mesh.cpp
    Source/Core/Renderer/Font.h
    Source/Core/Renderer/Font.cpp
    Source/Core/Renderer/Quad.h
    Source/Core/Renderer/Quad.cpp
)

add_library(Core STATIC)

target_sources(Core PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

target_link_libraries(Core glfw)
target_link_libraries(Core glad)
target_link_libraries(Core glm)
target_link_libraries(Core freetype)

target_include_directories(Core PUBLIC "Source" "Vendor/stb")