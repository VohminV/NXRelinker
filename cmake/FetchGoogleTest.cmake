include_guard(GLOBAL)

function(nxrelinker_add_googletest)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        message(STATUS "Using system GoogleTest ${GTest_VERSION}")
        return()
    endif()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST PARENT_SCOPE)
endfunction()
