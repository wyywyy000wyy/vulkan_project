
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(TARGET_NAME VulkanLib)
find_package (Vulkan REQUIRED)
include_directories(SYSTEM /usr/local/include)
target_include_directories(${TARGET_NAME} PRIVATE
/usr/local/include)

add_library(${TARGET_NAME} STATIC ${SOURCES})
if (WIN32)
target_link_libraries (${TARGET_NAME} Vulkan::Vulkan)
set_target_properties (${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET_NAME})
else()
set_target_properties (${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET_NAME})
target_include_directories(${TARGET_NAME} PRIVATE
  ${COMMON_DIR}/vulkan_wrapper)
endif ()