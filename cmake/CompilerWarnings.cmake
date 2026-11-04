include_guard(GLOBAL)

function(nxrelinker_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            /utf-8
            /Zc:__cplusplus
            /Zc:preprocessor
        )
        if(NXRELINKER_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wundef
            -Wformat=2
            -Wnull-dereference
            -Wdouble-promotion
            -Woverloaded-virtual
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
        )
        if(NXRELINKER_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
