cmake_minimum_required(VERSION 3.8)
project(poly)

include(GNUInstallDirs)
include(CTest)

add_subdirectory(external)

add_library(poly-headers INTERFACE)
target_sources(poly-headers INTERFACE ${poly_includes})
target_include_directories(poly-headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/poly>
        )
target_compile_features(poly-headers INTERFACE cxx_std_17)
target_link_libraries(poly-headers INTERFACE poly::etl)

add_library(poly::headers ALIAS poly-headers)

if (NOT POLY_PLATFORM)
    set(POLY_PLATFORM "PC")
endif()

string(TOUPPER ${POLY_PLATFORM} POLY_PLATFORM)

message("Building poly for platform ${POLY_PLATFORM}")

add_library(poly STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/panic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/md5.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/power.cpp
)
target_link_libraries(poly PUBLIC poly::headers)
target_compile_definitions(poly PUBLIC POLY_PLATFORM_${POLY_PLATFORM})
add_library(poly::poly ALIAS poly)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/poly
)

install(TARGETS poly ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/poly)

if (NOT POLY_SKIP_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
