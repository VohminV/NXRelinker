include_guard(GLOBAL)

function(nxrelinker_configure_sanitizers)
    if(NOT NXRELINKER_ENABLE_SANITIZERS)
        return()
    endif()

    if(MSVC)
        message(FATAL_ERROR
            "NXRELINKER_ENABLE_SANITIZERS is currently supported only with GCC and Clang on "
            "Linux/macOS. Use a Debug build with MSVC's own tooling instead.")
    endif()

    message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endfunction()
