cmake_minimum_required (VERSION 3.8.2)
project (ToyEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_C_FLAGS "/Zi /MP")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od")
set(CMAKE_C_FLAGS_RELEASE "/MT")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT")
set(CMAKE_CXX_FLAGS "-wd4351 -wd4530 -wd4577 -DWIN64=1 /EHsc /Zi /MP /std:c++latest")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od")
set(CMAKE_CXX_FLAGS_RELEASE "-DAK_OPTIMIZED=1 /MT")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DAK_OPTIMIZED=1 /MT")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# ---- SDL 2 ----	
if (WIN32)
	set(HAVE_XINPUT_H 1)
	link_directories($ENV{DXSDK_DIR}Lib\\x64)
endif (WIN32)

set(SDL_AUDIO ON CACHE BOOL "" FORCE)
set(DISKAUDIO OFF CACHE BOOL "" FORCE)
set(DUMMYAUDIO OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_POWER ON CACHE BOOL "" FORCE)
set(SDL_THREADS ON CACHE BOOL "" FORCE)
set(SDL_FILE ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(LIBC ON CACHE BOOL "" FORCE)

add_subdirectory(External/SDL2 EXCLUDE_FROM_ALL)

set(SDL_LIBS
	SDL2-static
	SDL2main
)

#--- Easy Profiler ---
add_subdirectory(External/EasyProfiler EXCLUDE_FROM_ALL)

#--- rapidjson ---
add_subdirectory(External/rapidjson EXCLUDE_FROM_ALL)

#--- Dear ImGui ---
add_library(
	imgui STATIC
	External/imgui/imconfig.h
	External/imgui/imgui.cpp
	External/imgui/imgui.h
	External/imgui/examples/imgui_impl_dx12.h
	External/imgui/examples/imgui_impl_dx12.cpp
	External/imgui/examples/imgui_impl_win32.h
	External/imgui/examples/imgui_impl_win32.cpp
	External/imgui/imgui_demo.cpp
	External/imgui/imgui_draw.cpp
	External/imgui/imgui_internal.h
	External/imgui/stb_rect_pack.h
	External/imgui/stb_textedit.h
	External/imgui/stb_truetype.h
)
target_include_directories(imgui PUBLIC External/imgui External/imgui/examples)

#---- DX 12 -----
#find_package(D3D12 REQUIRED)


#---INCLUDES----
set(IMPORT_INCLUDES 
	External/TinyObjLoader
	External/px_sched
	External/EasyProfiler/easy_profiler_core/include
	External/rapidjson/include/rapidjson
	External/imgui
	External/imgui/examples
	${SDL2_BINARY_DIR}/include
	${SDL2_SOURCE_DIR}/include
	${D3D12_INCLUDE_DIRS}
)

#--- Main project
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.inl")

add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

add_executable(ToyEngine ${SOURCES} ${HEADERS})

add_custom_command(TARGET ToyEngine PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Assets/ $<TARGET_FILE_DIR:ToyEngine>/Assets/
)

add_custom_command(TARGET ToyEngine PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Shaders/ $<TARGET_FILE_DIR:ToyEngine>/Shaders/
)

#message("WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}")
set_target_properties(ToyEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

target_include_directories(ToyEngine PRIVATE ${IMPORT_INCLUDES})
target_link_libraries(ToyEngine ${SDL_LIBS} dxgi.lib d3d12.lib d3dcompiler.lib imgui)
