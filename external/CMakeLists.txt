cmake_minimum_required(VERSION 3.26)

message(STATUS "---> glad building")
add_subdirectory(glad EXCLUDE_FROM_ALL)

message(STATUS "---> glfw building")
add_subdirectory(glfw EXCLUDE_FROM_ALL)

message(STATUS "---> glm building")
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++ 20 for GLM" FORCE)
add_subdirectory(glm EXCLUDE_FROM_ALL)

message(STATUS "---> stb_image building")
add_subdirectory(stb EXCLUDE_FROM_ALL)

# utfcpp header-only, so included directly

message(STATUS "---> tinygltf building")
add_subdirectory(tinygltf EXCLUDE_FROM_ALL)

# rapidjson header-only, so included directly
