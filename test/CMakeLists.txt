
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
add_test(NAME poly-test COMMAND poly-test)