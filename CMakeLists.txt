cmake_minimum_required(VERSION 3.28)
project(WireboundWorldCreator)

cmake_policy(SET CMP0069 NEW) # LTO support
cmake_policy(SET CMP0091 NEW) # MSVC runtime support
cmake_policy(SET CMP0075 NEW) # honor to CMAKE_REQUIRED_LIBRARIES

# for WireboundWorldCreator/external/
cmake_policy(SET CMP0077 NEW) # honors normal variables

set(CMAKE_WARN_DEPRECATED OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external EXCLUDE_FROM_ALL)

add_executable(WireboundWorldCreator
        src/main.cpp
        src/common/Shader.cpp
        src/common/Texture.cpp
        src/core/Map.cpp
        src/core/Interface.cpp
        src/core/Menu.cpp
        src/core/Tile.cpp
        src/core/TileRenderer.cpp
        src/core/Ui.cpp
        src/core/WireboundWorldCreator.cpp
        src/io/Camera.cpp
        src/io/Cursor.cpp
        src/io/Window.cpp
        src/modes/BiomesMode.cpp
        src/modes/FencesMode.cpp
        src/modes/ObjectsMode.cpp
        src/modes/PlacementMode.cpp
        src/modes/RoadsMode.cpp
        src/modes/SharedResources.cpp
        src/modes/TerrainMode.cpp
        src/modes/TilesMode.cpp
        src/modes/WaterMode.cpp
        src/renderers/PlacementRenderer.cpp
        src/renderers/TerrainRenderer.cpp
        src/renderers/WaterRenderer.cpp
        src/renderers/placement/Grass.cpp

        # temp (need to remove)
        src/common/ArbitraryGraph.cpp
)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(WireboundWorldCreator
        PUBLIC ${EXTERNAL_DIR}/glad/include
        PUBLIC ${EXTERNAL_DIR}/glfw/include
        PUBLIC ${EXTERNAL_DIR}/utfcpp
        PUBLIC ${EXTERNAL_DIR}/stb
        PRIVATE ${CMAKE_SOURCE_DIR}/external/rapidjson
        PRIVATE ${CMAKE_SOURCE_DIR}/external/tinygltf
)

target_link_libraries(WireboundWorldCreator
        PUBLIC glad
        PUBLIC glfw
        PUBLIC stb
        PRIVATE tinygltf
)

# TODO: rapidjson don't like new gcc...
# c++23 installed here on my pc; you better to replace or just remove
#target_include_directories(WireboundWorldCreator
#        PUBLIC /usr/local/include/c++/14.1.0
#)
#target_link_directories(WireboundWorldCreator
#        PUBLIC /usr/local/lib/gcc/x86_64-pc-linux-gnu/14.1.0
#)

#set_target_properties(WireboundWorldCreator PROPERTIES
#        BUILD_RPATH "/usr/local/lib64"
#        INSTALL_RPATH "/usr/local/lib64"
#)