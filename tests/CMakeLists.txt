include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Threads REQUIRED)

include(GoogleTest)

add_executable(decree_test
    decree_test.cpp
)

target_link_libraries(decree_test PRIVATE
    decree::decree
    GTest::gtest_main
    Threads::Threads
)

set_target_properties(decree_test PROPERTIES CXX_EXTENSIONS OFF)
gtest_discover_tests(decree_test)
