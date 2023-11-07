cmake_minimum_required (VERSION 3.8)
project (VulkanProject)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# find_package (glm REQUIRED)
find_package (Vulkan REQUIRED)
# find_package (tinyobjloader REQUIRED)

if (APPLE)
set(GLFW3_INCLUDE_PATH /opt/homebrew/Cellar/glfw/3.3.4/include)
set(GLFW3_LIB_PATH /opt/homebrew/Cellar/glfw/3.3.4/lib)
else()
set(GLFW3_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/dep/GLFW/include)
set(GLFW3_LIB_PATH ${PROJECT_SOURCE_DIR}/dep/GLFW/lib)
endif()


set(STB_INCLUDEDIR ${PROJECT_SOURCE_DIR}/dep/stb_image.h)

add_executable (glslang::validator IMPORTED)
find_program (GLSLANG_VALIDATOR "glslangValidator" HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
set_property (TARGET glslang::validator PROPERTY IMPORTED_LOCATION "${GLSLANG_VALIDATOR}")

function (add_shaders_target TARGET)
  cmake_parse_arguments ("SHADER" "" "CHAPTER_NAME" "SOURCES" ${ARGN})
  set (SHADERS_DIR ${SHADER_CHAPTER_NAME}/shaders)
  add_custom_command (
    OUTPUT ${SHADERS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    )
  add_custom_command (
    OUTPUT ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv
    COMMAND glslang::validator
    ARGS --target-env vulkan1.0 ${SHADER_SOURCES} --quiet
    WORKING_DIRECTORY ${SHADERS_DIR}
    DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
    COMMENT "Compiling Shaders"
    VERBATIM
    )
  add_custom_target (${TARGET} DEPENDS ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv)
endfunction ()

cmake_parse_arguments (CHAPTER "" "SHADER" "LIBS;TEXTURES;MODELS" ${ARGN})


add_subdirectory(vulkan_lib)

set(SUB_PRJ_INCLUDE_DIRS ${STB_INCLUDEDIR} ${GLFW3_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vulkan_lib /usr/local/include )


set(CHAPTER_NAME VulkanApp)

file(GLOB_RECURSE CHAPTER_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_library(${CHAPTER_NAME} STATIC ${CHAPTER_SOURCES}) 
target_link_libraries(${CHAPTER_NAME} VulkanLib)

  if (WIN32)
    set_target_properties (${CHAPTER_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CHAPTER_NAME})
    set_target_properties(${CHAPTER_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${CHAPTER_NAME})
  else()
    set_target_properties (${CHAPTER_NAME} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CHAPTER_NAME})
      
  endif ()
set_target_properties (${CHAPTER_NAME} PROPERTIES CXX_STANDARD 17)
# link_directories(${GLFW3_LIB_PATH})
# link_libraries(${GLFW3_LIB_PATH})
target_link_directories(${CHAPTER_NAME} PRIVATE ${GLFW3_LIB_PATH})
# target_link_libraries (${CHAPTER_NAME} Vulkan::Vulkan glfw3)
target_include_directories (${CHAPTER_NAME} PRIVATE ${SUB_PRJ_INCLUDE_DIRS} )

if (DEFINED CHAPTER_SHADER)
  set (CHAPTER_SHADER_TARGET ${CHAPTER_NAME}_shader)
  file (GLOB SHADER_SOURCES ${CHAPTER_SHADER}.frag ${CHAPTER_SHADER}.vert)
  add_shaders_target (${CHAPTER_SHADER_TARGET} CHAPTER_NAME ${CHAPTER_NAME} SOURCES ${SHADER_SOURCES})
  add_dependencies (${CHAPTER_NAME} ${CHAPTER_SHADER_TARGET})
endif ()
if (DEFINED CHAPTER_LIBS)
  target_link_libraries (${CHAPTER_NAME} ${CHAPTER_LIBS})
endif ()
if (DEFINED CHAPTER_MODELS)
  file (COPY ${CHAPTER_MODELS} DESTINATION ${CMAKE_BINARY_DIR}/${CHAPTER_NAME}/models)
endif ()
if (DEFINED CHAPTER_TEXTURES)
  file (COPY ${CHAPTER_TEXTURES} DESTINATION ${CMAKE_BINARY_DIR}/${CHAPTER_NAME}/textures)
endif ()

# add_executable(VulkanProject main.cpp)
# target_link_libraries(VulkanProject VulkanLib)
# target_include_directories(VulkanProject PRIVATE ${STB_INCLUDEDIR} ${GLFW3_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/src)




set(LIB_ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_lib/assets)
set(LIB_SHADER_PATH ${LIB_ASSET_PATH}/shaders)
file (GLOB LIB_SHADER_SOURCES ${LIB_SHADER_PATH}/*.frag ${LIB_SHADER_PATH}/*.vert)
message(***************)
message(${LIB_ASSET_PATH})
foreach(SHADER_SOURCE ${LIB_SHADER_SOURCES})
    message(${SHADER_SOURCE})
endforeach()
message(***************)


set(LIB_ASSET_DIR ${CMAKE_BINARY_DIR}/assets)
set(LIB_SHADER_DIR ${LIB_ASSET_DIR}/shaders)

message(${LIB_SHADER_DIR})

function (add_shaders_target2 TARGET)
    add_custom_command (
        OUTPUT ${LIB_SHADER_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_SHADER_DIR}
        )
    add_custom_command (
            OUTPUT ${LIB_SHADER_DIR}/frag.spv ${LIB_SHADER_DIR}/vert.spv
            COMMAND glslang::validator
            ARGS --target-env vulkan1.0 ${LIB_SHADER_SOURCES} --quiet
            WORKING_DIRECTORY ${LIB_SHADER_DIR}
            DEPENDS ${LIB_SHADER_DIR} ${LIB_SHADER_SOURCES}
            COMMENT "Compiling Shaders"
            VERBATIM
            )
    add_custom_target (${TARGET} DEPENDS ${LIB_SHADER_DIR}/frag.spv ${LIB_SHADER_DIR}/vert.spv)
endfunction()

set(SUB_PRJ_LINK_LIBS VulkanApp Vulkan::Vulkan glfw3)

function(add_sub_prj SUB_PRJECT)
    get_filename_component(SUB_PRJECT_NAME ${SUB_PRJECT} NAME)
    file(GLOB_RECURSE SUB_PRJECT_SOURCES ${SUB_PRJECT}/*.cpp ${SUB_PRJECT}/*.h)
    add_executable(${SUB_PRJECT_NAME} ${SUB_PRJECT_SOURCES})
    target_link_directories(${SUB_PRJECT_NAME} PRIVATE ${GLFW3_LIB_PATH})
    target_link_libraries(${SUB_PRJECT_NAME} ${SUB_PRJ_LINK_LIBS})
    target_include_directories(${SUB_PRJECT_NAME} PRIVATE ${SUB_PRJ_INCLUDE_DIRS})

    set (CHAPTER_SHADER_TARGET ${SUB_PRJECT_NAME}_shader)
    add_shaders_target2(${CHAPTER_SHADER_TARGET})
    add_dependencies(${SUB_PRJECT_NAME} ${CHAPTER_SHADER_TARGET})
endfunction(add_sub_prj)

file(GLOB SUB_PRJECTS ${PROJECT_SOURCE_DIR}/prj/*)
foreach(SUB_PRJECT ${SUB_PRJECTS})
    add_sub_prj(${SUB_PRJECT})
endforeach()