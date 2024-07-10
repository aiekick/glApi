set(IMGUIPACK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ImGuiPack)
set(IMGUIPACK_LIBRARIES ImGuiPack)

# ON
set(USE_IM_TOOLS ON CACHE BOOL "" FORCE)
set(USE_IMGUI_FILE_DIALOG ON CACHE BOOL "" FORCE)
set(USE_IMGUI_COLOR_TEXT_EDIT ON CACHE BOOL "" FORCE)
set(IMGUIPACK_USE_STD_FILESYSTEM ON CACHE BOOL "" FORCE)

# OFF
set(USE_IMPLOT OFF CACHE BOOL "" FORCE)
set(USE_IM_GUIZMO OFF CACHE BOOL "" FORCE)
set(USE_IMCOOL_BAR OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_MARKDOW OFF CACHE BOOL "" FORCE)
set(USE_IM_GRADIENT_HDR OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_NODE_EDITOR OFF CACHE BOOL "" FORCE)
set(USE_IN_APP_GPU_PROFILER OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack)

if(USE_SHARED_LIBS)
	set_target_properties(${IMGUIPACK_LIBRARIES} PROPERTIES FOLDER 3rdparty/Shared)
	set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES FOLDER 3rdparty/Shared)
else()
	set_target_properties(${IMGUIPACK_LIBRARIES} PROPERTIES FOLDER 3rdparty/Static)
	set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES FOLDER 3rdparty/Static)
endif()

set_target_properties(${IMGUIPACK_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}")
set_target_properties(${IMGUIPACK_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${IMGUIPACK_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")

set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}")
set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${FREETYPE_LIBRARIES} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")

add_definitions(-DUSE_IM_TOOLS)
add_definitions(-DUSE_IMGUI_FILE_DIALOG)
add_definitions(-DUSE_IMGUI_COLOR_TEXT_EDIT)
add_definitions(-DIMGUIPACK_USE_STD_FILESYSTEM)
