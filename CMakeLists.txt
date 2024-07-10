cmake_minimum_required(VERSION 3.11)

set(PROJECT glApiDemoApp)

enable_language(C CXX)
project(${PROJECT} CXX)

option(USE_BUILDING_AS_SHARED "Enable shared building" OFF)
option(USE_BUILDING_OF_TESTS "Enable the build of tests" OFF)

if (USE_BUILDING_AS_SHARED)
    set(USE_SHARED_LIBS ON)
else()
    set(USE_SHARED_LIBS OFF)
endif ()

## for group smake targets in the dir CmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeTargets")

## show the dependency graph of cmake
## set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}/debug")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}/release")
set(CMAKE_PDB_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}/minsizerel")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}/reldeb")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

set(USE_SHARED_LIBS ${USE_BUILDING_AS_SHARED} CACHE BOOL "" FORCE)

# some times for unknown reason, the lib file is not generated so no symbols are exported.
# this option force all sahred lib to export theirs symbols
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ${USE_SHARED_LIBS} CACHE BOOL "" FORCE)
set(ARCH x64)

if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX _debug)
endif ()
if (NOT CMAKE_RELEASE_POSTFIX)
    set(CMAKE_RELEASE_POSTFIX) ## to config in file
endif ()
if (NOT CMAKE_MINSIZEREL_POSTFIX)
    set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)
endif ()
if (NOT CMAKE_RELWITHDEBINFO_POSTFIX)
    set(CMAKE_RELWITHDEBINFO_POSTFIX _reldeb)
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zo /Ox")
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DMSVC)
    add_definitions(-DNOMINMAX)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wunused-parameter -Wno-unknown-pragmas -O3 -m64 -fno-rtti")
endif ()

if (WIN32)
    add_definitions(-DWIN32)
endif ()

add_definitions(-DUSE_XML_CONFIG)
add_definitions(-DUSE_STD_FILESYSTEM)
add_definitions(-DIMGUI_ENABLE_FREETYPE)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
add_definitions(-DUSE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS)
add_definitions(-DIMGUI_INCLUDE="${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/ImGuiPack.h")
add_definitions(-DIMGUI_USER_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomImGuiConfig.h")
add_definitions(-DCUSTOM_IMWIDGETS_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomImWidgetsConfig.h")
add_definitions(-DCUSTOM_IMGUIFILEDIALOG_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomImGuiFileDialogConfig.h")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

include(cmake/3rdparty.cmake)

###########################################################
###########################################################
###########################################################

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# App Sources
file(GLOB_RECURSE SRC_SOURCES ${CMAKE_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${SRC_SOURCES})

# cmake Sources for have it the visual studio tree of the App
file(GLOB_RECURSE PROJECT_CMAKE_FILES ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
source_group(TREE ${CMAKE_SOURCE_DIR}/cmake PREFIX cmake FILES ${PROJECT_CMAKE_FILES})

file(GLOB IMGUI_IMPL_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/misc/freetype/imgui_freetype.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/misc/freetype/imgui_freetype.h)
source_group(src/ImGuiImpl FILES ${IMGUI_IMPL_SOURCES})

add_executable(${PROJECT}
    ${SRC_SOURCES}
    ${IMGUI_IMPL_SOURCES}
    ${PROJECT_CMAKE_FILES}
)

if (USE_SHARED_LIBS)
    target_compile_definitions(${PROJECT} PRIVATE BUILD_IMGUI_PACK_SHARED_LIBS)
    target_compile_definitions(${PROJECT} PRIVATE BUILD_IN_APP_GPU_PROFILER_SHARED_LIBS)
    target_compile_definitions(${PROJECT} PRIVATE BUILD_CASHME_BRIDGE_SHARED_LIBS)
endif ()

set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_${CMAKE_SYSTEM_NAME}_${ARCH}")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})

target_link_libraries(${PROJECT}
    ${IMGUIPACK_LIBRARIES}
    ${TINYXML2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${CTOOLS_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
)

target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CTOOLS_INCLUDE_DIR}
)
# define for let CashMe detect what is the config of a plugin
target_compile_definitions(${PROJECT} PRIVATE -DPLUGIN_RUNTIME_CONFIG=$<$<CONFIG:Debug>:"_Debug">$<$<CONFIG:Release>:"_Release">$<$<CONFIG:MinSizeRel>:"_MinSizeRel">$<$<CONFIG:RelWithDebInfo>:"_RelWithDebInfo">)
target_compile_definitions(${PROJECT} PRIVATE -DPROJECT_EXT=".cash")
target_compile_definitions(${PROJECT} PRIVATE -DPROJECT_EXT_DOT_LESS="cash")

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

########################################################
##### TESTS ############################################
########################################################

if (USE_BUILDING_OF_TESTS)
    enable_testing()
    add_definitions(-DTEST_ENABLED)
    add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif ()

