include(FetchGoogleTest)
nxrelinker_add_googletest()

add_executable(nxrelinker_tests
    binary/test_elf.cpp
    binary/test_format_detection.cpp
    binary/test_inspection.cpp
    binary/test_nro.cpp
    binary/test_nso.cpp
    unit/test_checked_math.cpp
    unit/test_diagnostics.cpp
    unit/test_endian.cpp
)

target_include_directories(nxrelinker_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/binary")
target_link_libraries(nxrelinker_tests PRIVATE nxrelinker GTest::gtest GTest::gtest_main)
target_compile_features(nxrelinker_tests PRIVATE cxx_std_20)
nxrelinker_apply_warnings(nxrelinker_tests)

include(GoogleTest)
gtest_discover_tests(nxrelinker_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES TIMEOUT 30
)

add_test(NAME nxinspect.version COMMAND nxinspect --version)
add_test(NAME nxinspect.help COMMAND nxinspect --help)

add_test(NAME nxinspect.invalid_option COMMAND nxinspect --not-an-option)
set_tests_properties(nxinspect.invalid_option PROPERTIES WILL_FAIL TRUE)
