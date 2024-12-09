include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# Configure GoogleTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(tsid_tests test_uutid.cpp)

target_link_libraries(tsid_tests
    PRIVATE
        tsid
        gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(tsid_tests)
