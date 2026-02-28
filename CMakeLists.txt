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
        src/render/Shader.cpp
        src/common/TextRenderer.cpp
        src/common/Vbos.cpp
        src/core/TileRenderer.cpp
        src/core/WireboundWorldCreator.cpp
        src/io/Window.cpp
        src/modes/UiBiomesMode.cpp
        src/modes/UiObjectsMode.cpp
        src/modes/UiPlacementMode.cpp
        src/modes/UiTerrainMode.cpp
        src/modes/UiTilesMode.cpp
        src/modes/UiWaterMode.cpp
        src/renderers/TerrainRenderer.cpp
        src/renderers/WaterRenderer.cpp
        src/renderers/Ifft.cpp
        src/renderers/Ocean.cpp

        # temp (need to remove)
        src/core/Environment.cpp
        src/core/Environment.h
        src/common/SkylineBottomLeft.h
        src/common/LocalTransform.h
        src/common/Font.h
        src/common/Text.h
        src/common/Text.cpp
        src/common/GlfwContext.cpp
        src/core/UiSlots.cpp
        src/core/UiSlots.h
        src/core/UiComplex.cpp
        src/core/UiComplex.h
        src/modes/UiTerrainConfig.h
        src/core/WindowQueue.h
        src/core/WindowQueue.cpp
        src/core/UiText.cpp
        src/core/UiText.h
        src/core/Tile.cpp
        src/modes/TerrainNoiseData.h
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
        src/common/models/PlayerFpv.cpp
        src/common/models/PlayerFpv.h
        src/common/models/PlayerHuman.cpp
        src/common/models/PlayerHuman.h
        src/common/models/RigidBody.cpp
        src/common/models/RigidBody.h
        src/common/models/Obstacle.cpp
        src/common/models/Obstacle.h
        src/common/models/ModelManager.cpp
        src/common/models/ModelManager.h
        src/common/MouseTransform.cpp
        src/common/MouseTransform.h
        src/common/BaseInstanceData.h
        src/common/Callbacks.cpp
        src/common/Callbacks.h
        src/modes/UiEditOcean.cpp
        src/modes/UiEditOcean.h
        src/modes/UiEditRiver.cpp
        src/modes/UiEditRiver.h
        src/modes/UiEdit.cpp
        src/modes/UiEdit.h
        src/modes/UiEditBiomes.cpp
        src/modes/UiEditBiomes.h
        src/modes/UiEditObjects.cpp
        src/modes/UiEditObjects.h
        src/modes/UiEditShared.cpp
        src/modes/UiEditShared.h
        src/core/UiConfigWindow.cpp
        src/core/UiConfigWindow.h
        src/modes/UiOceanConfig.h
        src/modes/UiTerrainConfig.cpp
        src/common/Material.h
        src/common/GaussianNoise.cpp
        src/common/PickingFramebuffer.cpp
        src/common/MapPoint.h
        src/core/UiMenu.cpp
        src/core/UiMenu.h
        src/common/MapPoints.cpp
        src/common/MapPoints.h
        src/modes/UiEditOcean.cpp
        src/modes/UiEditOcean.h
        src/modes/UiOceanConfig.h
        src/modes/UiEditRiver.cpp
        src/modes/UiEditRiver.h
        src/modes/UiEditTerrain.cpp
        src/modes/UiEditTerrain.h
        src/modes/UiTerrainBake.cpp
        src/modes/UiTerrainBake.h
        src/common/GraphBakeConfig.h
        src/modes/UiEdit.cpp
        src/modes/UiEditRoads.cpp
        src/modes/UiEditRoads.h
        src/common/models/Animator.cpp
        src/common/models/Animator.h
        src/common/models/Aabb3D.cpp
        src/common/models/Aabb3D.h
        src/common/EntityIdManager.cpp
        src/common/EntityIdManager.h
        src/common/models/AttackEvent.h
        src/common/models/MapMarker.cpp
        src/common/models/MapMarker.h
        src/renderers/Vegetation.cpp
        src/renderers/Vegetation.h
        src/common/models/Fpv.cpp
        src/common/models/Fpv.h
        src/common/models/Human.cpp
        src/common/models/Human.h
        src/modes/traits/BiomeTraits.h
        src/modes/traits/RoadTraits.h
        src/modes/traits/RiverTraits.h
        src/modes/traits/ObjectTraits.h
        src/modes/traits/OceanTraits.h
        src/core/UiModal.cpp
        src/core/UiModal.h
        src/renderers/GeoClipmaps.cpp
        src/renderers/GeoClipmaps.h
        src/render/Texture.cpp
        src/render/Texture.h
        src/modes/ModeTerrain.h
        src/ui/UIElement.cpp
        src/ui/UIElement.h
        src/ui/UiRenderer.cpp
        src/ui/UiRenderer.h
        src/ui/UiQuad.h
        src/render/Font.cpp
        src/render/Font.h
        src/render/DebugRendererImp.cpp
        src/render/DebugRendererImp.h
        src/render/Frustum.h
        src/render/Renderer.cpp
        src/render/Renderer.h
        src/core/Wirebound.cpp
        src/core/Wirebound.h
        src/core/Layers.h
        src/ui/UiQuad.cpp
        src/ui/UiSprite.cpp
        src/ui/UiSprite.h
        src/ui/UiButton.cpp
        src/ui/UiButton.h
        src/ui/UiSlider.cpp
        src/ui/UiSlider.h
        src/ui/UiText.cpp
        src/ui/UiText.h
        src/ui/UiCheckBox.cpp
        src/ui/UiCheckBox.h
        src/ui/UiRadioButton.cpp
        src/ui/UiRadioButton.h
        src/ui/UiAnimation.cpp
        src/ui/UiAnimation.h
        src/ui/custom/UiCompass.cpp
        src/ui/custom/UiCompass.h
        src/ui/custom/UiGameMap.cpp
        src/ui/custom/UiGameMap.h
        src/ui/custom/UiSettings.cpp
        src/ui/custom/UiSettings.h
        src/ui/custom/UiObjectInfo.cpp
        src/ui/custom/UiObjectInfo.h
)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(WireboundWorldCreator
        PUBLIC ${EXTERNAL_DIR}/glad/include
        PUBLIC ${EXTERNAL_DIR}/glfw/include
        PUBLIC ${EXTERNAL_DIR}/glm/glm
        PUBLIC ${EXTERNAL_DIR}/utfcpp
        PUBLIC ${EXTERNAL_DIR}/JoltPhysics-5.5.0/Jolt
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
        PUBLIC nanosvg
        PUBLIC Jolt
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