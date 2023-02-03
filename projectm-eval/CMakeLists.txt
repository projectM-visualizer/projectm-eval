
set(BISON_OUTPUT_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.h
        )

set(FLEX_OUTPUT_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Scanner.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Scanner.h
        )

if(FLEX_FOUND)
    # Generate lexer code
    add_custom_command(COMMAND ${FLEX_EXECUTABLE}
            -o ${CMAKE_CURRENT_SOURCE_DIR}/Scanner.c
            --prefix=prjm_eel_
            --header-file=${CMAKE_CURRENT_SOURCE_DIR}/Scanner.h
            Scanner.l
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT ${FLEX_OUTPUT_FILES}
            DEPENDS Scanner.l
            )

    add_custom_target(GenerateScanner ALL
        DEPENDS ${FLEX_OUTPUT_FILES}
        )
endif()

if(BISON_FOUND)
    # Generate compiler code
    add_custom_command(COMMAND ${BISON_EXECUTABLE}
            -o ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.c
            --defines=${CMAKE_CURRENT_SOURCE_DIR}/Compiler.h
            -Wcounterexamples
            Compiler.y
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT ${BISON_OUTPUT_FILES}
            DEPENDS Compiler.y
            )

    add_custom_target(GenerateCompiler ALL
            DEPENDS ${BISON_OUTPUT_FILES}
            )
endif()

add_library(projectM_EvalLib STATIC
        ${BISON_OUTPUT_FILES}
        ${FLEX_OUTPUT_FILES}
        CompileContext.c
        CompileContext.h
        Compiler.y
        CompilerFunctions.c
        CompilerFunctions.h
        CompilerTypes.h
        ExpressionTree.c
        ExpressionTree.h
        MemoryBuffer.c
        MemoryBuffer.h
        Scanner.l
        TreeFunctions.c
        TreeFunctions.h
        TreeVariables.c
        TreeVariables.h
        api/projectm-eval.c
        api/projectm-eval.h
        )

target_include_directories(projectM_EvalLib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        )

target_link_libraries(projectM_EvalLib
        INTERFACE
        m
        )