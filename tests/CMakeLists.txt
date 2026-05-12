include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Threads REQUIRED)

add_executable(condair_error_handling_test
    condair_error_handling_test.cpp
)

target_link_libraries(condair_error_handling_test
    PRIVATE
        condair::error_handling_library
        GTest::gtest_main
        Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(condair_error_handling_test)
