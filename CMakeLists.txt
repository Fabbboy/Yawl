cmake_minimum_required(VERSION 3.20)
project(yawl)

if(UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cc")
file(GLOB_RECURSE HEADERS "${INC_DIR}/*.h")

add_library(yawl STATIC ${SOURCES} ${HEADERS})
target_include_directories(yawl PUBLIC ${INC_DIR})

if(UNIX AND NOT APPLE) 
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(XCB xcb)
    pkg_check_modules(WAYLAND wayland-client)

    if(XCB_FOUND)
        target_include_directories(yawl PUBLIC ${XCB_INCLUDE_DIRS})
        target_link_libraries(yawl ${XCB_LIBRARIES})
        target_compile_definitions(yawl PUBLIC -DHAVE_X11)
    endif()

    if(WAYLAND_FOUND)
        target_include_directories(yawl PUBLIC ${WAYLAND_INCLUDE_DIRS})
        target_link_libraries(yawl ${WAYLAND_LIBRARIES})
        target_compile_definitions(yawl PUBLIC -DHAVE_WAYLAND)
    endif()
endif()

add_executable(yawl_example "${CMAKE_CURRENT_SOURCE_DIR}/example.cc")
target_link_libraries(yawl_example yawl)
