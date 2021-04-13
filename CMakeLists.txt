cmake_minimum_required (VERSION 3.12)
cmake_policy(VERSION 3.12)

project (hallow_streets)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
#find_package(PHYSX REQUIRED)
#message(STATUS "Found PHYSX in ${ASSIMP_INCLUDE_DIR}")

find_package(Boost 1.74.0 REQUIRED COMPONENTS
             date_time filesystem iostreams)
message(STATUS "Found Boost in ${Boost_INCLUDE_DIR}")
message(STATUS "Found Boost libs in ${Boost_LIBRARY_DIR}")
message(STATUS "Found Boost includes in ${Boost_INCLUDE_DIRS}")
# find_package(SOIL REQUIRED)
# message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")

if(WIN32)
  set(LIBS glfw3 opengl32 assimp freetype irrKlang 
  ${CMAKE_SOURCE_DIR}/lib/PhysX_64.lib
  ${CMAKE_SOURCE_DIR}/lib/PhysXCommon_64.lib
  ${CMAKE_SOURCE_DIR}/lib/PhysXFoundation_64.lib
  ${CMAKE_SOURCE_DIR}/lib/PhysXExtensions_static_64.lib
  ${CMAKE_SOURCE_DIR}/lib/PhysXCooking_64.lib
  ${Boost_LIBRARY_DIR}/libboost_regex-vc142-mt-gd-x64-1_74.lib
  ${Boost_LIBRARY_DIR}/libboost_date_time-vc142-mt-gd-x64-1_74.lib
   ${Boost_LIBRARY_DIR}/libboost_chrono-vc142-mt-gd-x64-1_74.lib
  ${Boost_LIBRARY_DIR}/libboost_thread-vc142-mt-gd-x64-1_74.lib)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()


set(CHAPTERS
    engine
    dialog
    lighting_test
    mass_rendering_system
    networking_test
	client
	server
)

foreach(CHAPTER ${CHAPTERS})

file(GLOB_RECURSE SOURCE
    "src/${CHAPTER}/*.h"
    "src/${CHAPTER}/*.cpp"
    "src/${CHAPTER}/*.hpp"
    "src/${CHAPTER}/*.vs"
    "src/${CHAPTER}/*.fs"
    "src/${CHAPTER}/*.gs"
)

set(NAME "${CHAPTER}")
add_executable(${NAME} ${SOURCE} ${CMAKE_SOURCE_DIR}/resources/icon/icon.rc)
target_link_libraries(${NAME} ${LIBS})
if(WIN32)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/Debug")
elseif(UNIX AND NOT APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
elseif(APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
endif(WIN32)

# copy shader files to build directory
file(GLOB_RECURSE SHADERS
    "src/${CHAPTER}/*.vs"
    "src/${CHAPTER}/*.fs"
    "src/${CHAPTER}/*.gs"
)

foreach(SHADER ${SHADERS})
    if(WIN32)
        # configure_file(${SHADER} "test")
        add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
    elseif(UNIX AND NOT APPLE)
        file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${CHAPTER})
    elseif(APPLE)
        # create symbolic link for *.vs *.fs *.gs
        get_filename_component(SHADERNAME ${SHADER} NAME)
        makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/${SHADERNAME} ${NAME})
    endif(WIN32)
 endforeach(SHADER)

if(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
endif(MSVC)

#replicates the file structure in visual studio
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()
endforeach(CHAPTER)


include_directories(${CMAKE_SOURCE_DIR}/includes ${Boost_INCLUDE_DIRS})
