# Checks for fast math optimization compiler support if enabled.
# While this speeds up the generated code, there may be some side effects leading to incorrect math results.
# These should only affect edge cases, e.g. if ops result in NaN or Inf. This will in general not cause any
# visible artifacts, but may be undesirable in some uses. In this case, disable the ENABLE_FAST_MATH option
# and specify the safe set of flags via the CMAKE_C_FLAGS and CMAKE_CXX_FLAGS variables.

if(ENABLE_FAST_MATH)
    if(MSVC)
        add_compile_options($<$<CONFIG:Release,RelWithDebInfo>:/fp:fast>)
    else()
        add_compile_options($<$<CONFIG:Release,RelWithDebInfo>:-Ofast>)
    endif()
endif()
