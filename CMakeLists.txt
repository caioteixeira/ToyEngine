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

add_subdirectory(External/SDL2)

set(SDL_LIBS
	SDL2-static
	SDL2main
)

#--- Easy Profiler ---
add_subdirectory(External/EasyProfiler)

add_subdirectory(External/rapidjson)

#---- DX 12 -----
find_package(D3D12 REQUIRED)

set(IMPORT_INCLUDES 
	External/TinyObjLoader
	External/px_sched
	External/EasyProfiler/easy_profiler_core/include
	External/rapidjson/include/rapidjson
	${SDL2_BINARY_DIR}/include
	${SDL2_SOURCE_DIR}/include
	${D3D12_INCLUDE_DIRS}
)

#--- Main project
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.inl")

add_executable(ToyEngine ${SOURCES} ${HEADERS})

set_target_properties(ToyEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_include_directories(ToyEngine PRIVATE ${IMPORT_INCLUDES})
target_link_libraries(ToyEngine ${SDL_LIBS} ${D3D12_LIBRARIES})
