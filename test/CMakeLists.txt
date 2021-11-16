
if(NOT TARGET gtest_main)
    include(googletest.cmake)
endif()

file(GLOB test_sources *.cpp)

file(GLOB lib_sources ../src/*.cpp)

add_executable(poly-test
        ${test_sources}
        ${lib_sources}
        )
target_link_libraries(poly-test
        gtest_main
        poly::headers
        )
target_compile_definitions(poly-test PRIVATE POLY_PLATFORM_TESTING)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(poly-test PRIVATE -Wno-self-assign-overloaded)
endif()

add_test(NAME poly-test COMMAND poly-test)