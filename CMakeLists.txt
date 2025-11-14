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
        src/common/TextRenderer.cpp
        src/common/Vbos.cpp
        src/core/Menu.cpp
        src/core/TileRenderer.cpp
        src/core/WireboundWorldCreator.cpp
        src/io/Cursor.cpp
        src/io/Window.cpp
        src/modes/UiBiomesMode.cpp
        src/modes/UiFencesMode.cpp
        src/modes/UiObjectsMode.cpp
        src/modes/UiPlacementMode.cpp
        src/modes/UiRoadsMode.cpp
        src/modes/UiTerrainMode.cpp
        src/modes/UiTilesMode.cpp
        src/modes/UiWaterMode.cpp
        src/renderers/PlacementRenderer.cpp
        src/renderers/TerrainRenderer.cpp
        src/renderers/WaterRenderer.cpp
        src/renderers/placement/Grass.cpp
        src/renderers/water/Ifft.cpp
        src/renderers/water/WavesCascade.cpp
        src/renderers/water/Ocean.cpp

        # temp (need to remove)
        src/common/ArbitraryGraph.cpp
        src/environment/Wind.cpp
        src/environment/Wind.h
        src/environment/Time.cpp
        src/environment/Time.h
        src/environment/Environment.cpp
        src/environment/Environment.h
        src/common/OpenGlUtility.h
        src/core/Ui.cpp
        src/core/Ui.h
        src/core/Ui.cpp
        src/common/UiDebugger.cpp
        src/common/UiDebugger.h
        src/renderers/water/MacLabSwe.cpp
        src/renderers/water/MacLabSwe.h
        src/common/PlayerCharacter.h
        src/common/SkylineBottomLeft.h
        src/common/LocalTransform.h
        src/common/Font.h
        src/common/Text.h
        src/common/Text.cpp
        src/common/GlobalGlfwCallbackData.cpp
        src/core/UiSlots.cpp
        src/core/UiSlots.h
        src/renderers/UiRenderer.cpp
        src/renderers/UiRenderer.h
        src/modes/UiSharedResources.cpp
        src/modes/UiSharedResources.h
        src/modes/AllUiModes.h
        src/core/UiComplex.cpp
        src/core/UiComplex.h
        src/modes/UiTerrainConfig.cpp
        src/modes/UiTerrainConfig.h
        src/common/TerrainGrid.cpp
        src/common/TerrainGrid.h
        src/common/IGraph.h
        src/core/WindowQueue.h
        src/core/WindowQueue.cpp
        src/core/UiText.cpp
        src/core/UiText.h
        src/modes/UiTerrainWIndows.cpp
        src/modes/UiTerrainWIndows.h
        src/core/Tile.cpp
        src/modes/TerrainNoiseData.h
        src/modes/TerrainInstanceData.h
        src/core/UiLayerWireframe.cpp
        src/core/UiLayerWireframe.h
        src/core/UiWorldOrigin.cpp
        src/core/UiWorldOrigin.h
        src/core/UiButtons.cpp
        src/core/UiButtons.h
        src/core/UiGrid.cpp
        src/core/UiGrid.h
        src/core/UiSelection.cpp
        src/core/UiSelection.h
        src/modes/UiPlayerMode.cpp
        src/modes/UiPlayerMode.h
        src/io/Camera.cpp
        src/io/Camera.h
        src/common/models/ModelLoader.cpp
        src/common/models/ModelLoader.h
        src/common/models/Player.cpp
        src/common/models/Player.h
        src/common/models/Creature.cpp
        src/common/models/Creature.h
        src/common/models/RigidBody.cpp
        src/common/models/RigidBody.h
        src/common/models/Obstacle.cpp
        src/common/models/Obstacle.h
        src/common/models/ModelManager.cpp
        src/common/models/ModelManager.h

)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(WireboundWorldCreator
        PUBLIC ${EXTERNAL_DIR}/glad/include
        PUBLIC ${EXTERNAL_DIR}/glfw/include
        PUBLIC ${EXTERNAL_DIR}/glm/glm
        PUBLIC ${EXTERNAL_DIR}/utfcpp
#        PUBLIC ${EXTERNAL_DIR}/stb
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/rapidjson/include
#        PRIVATE ${CMAKE_SOURCE_DIR}/external/tinygltf
)

target_link_libraries(WireboundWorldCreator
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC stb
        PUBLIC rapidjson
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