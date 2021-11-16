include(FetchContent)

FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_SHALLOW ON
        GIT_TAG release-1.11.0 # release-1.11.0
        CMAKE_ARGS -DBUILD_TESTING=OFF
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)