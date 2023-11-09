
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(TARGET_NAME VulkanLib)
find_package (glm REQUIRED)
find_package (Vulkan REQUIRED)
include_directories(SYSTEM /usr/local/include)


add_library(${TARGET_NAME} STATIC ${SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE
/usr/local/include)
if (WIN32 OR APPLE)
target_link_libraries (${TARGET_NAME} Vulkan::Vulkan )
set_target_properties (${TARGET_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET_NAME})
else()
set_target_properties (${TARGET_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${TARGET_NAME})
target_include_directories(${TARGET_NAME} PRIVATE
  ${COMMON_DIR}/vulkan_wrapper)
endif ()


