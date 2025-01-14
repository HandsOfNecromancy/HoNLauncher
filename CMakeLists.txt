cmake_minimum_required(VERSION 3.10)
project(GameLauncher)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	set(EXTRA_LIBS ${COCOA_LIBRARY})
	set(SOURCE_FILES "macos.mm")
elseif(UNIX)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

	include_directories(${GTK3_INCLUDE_DIRS})
	link_directories(${GTK3_LIBRARY_DIRS})
	add_definitions(${GTK3_CFLAGS_OTHER})
	set(EXTRA_LIBS ${GTK3_LIBRARIES})
	set(SOURCE_FILES "gtk.cpp")
elseif(WIN32)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#	set(COMMON_LANGUAGE_RUNTIME pure)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
	set(EXTRA_LIBS "user32.lib")
	set(SOURCE_FILES "win32.cpp")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:WinMainCRTStartup")
endif()

set(SOURCE_FILES ${SOURCE_FILES} "launcher.cpp" "configfile.cpp" "getexepath.cpp")

add_executable(GameLauncher ${SOURCE_FILES})
target_link_libraries(GameLauncher ${EXTRA_LIBS})

if (WIN32)
	target_sources(GameLauncher PRIVATE GameLauncher.manifest)
	target_link_options(GameLauncher PRIVATE
		$<$<CONFIG:Debug>:${CMAKE_EXE_LINKER_FLAGS_DEBUG}>
		$<$<CONFIG:Release>:${CMAKE_EXE_LINKER_FLAGS_RELEASE}>
	)
endif()
