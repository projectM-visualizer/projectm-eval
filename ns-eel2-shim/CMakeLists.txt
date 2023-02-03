add_library(ns-eel2 STATIC
        ns-eel2.c
        ns-eel2.h
        )

target_compile_definitions(ns-eel2
        PUBLIC
        EEL_F_SIZE=${PROJECTM_EVAL_FLOAT_SIZE}
        )

target_include_directories(ns-eel2
        PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/projectm-eel
        INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ns-eel2
        )

