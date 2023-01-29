
set(BISON_OUTPUT_FILES
        Compiler.c
        Compiler.h
        )

set(FLEX_OUTPUT_FILES
        Scanner.c
        Scanner.h
        )

# Generate lexer code
add_custom_command(COMMAND ${FLEX_EXECUTABLE}
        -o ${CMAKE_CURRENT_BINARY_DIR}/Scanner.c
        --prefix=prjm_eel_
        --header-file=${CMAKE_CURRENT_BINARY_DIR}/Scanner.h
        Scanner.l
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT ${FLEX_OUTPUT_FILES}
        DEPENDS Scanner.l
        )

add_custom_target(GenerateScanner ALL
        DEPENDS ${FLEX_OUTPUT_FILES}
        )

# Generate compiler code
add_custom_command(COMMAND ${BISON_EXECUTABLE}
        -o ${CMAKE_CURRENT_BINARY_DIR}/Compiler.c
        --defines=${CMAKE_CURRENT_BINARY_DIR}/Compiler.h
        -Wcounterexamples
        Compiler.y
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT ${BISON_OUTPUT_FILES}
        DEPENDS Compiler.y
        )

add_custom_target(GenerateCompiler ALL
        DEPENDS ${BISON_OUTPUT_FILES}
        )

add_executable(SimpleCompiler
        CompileContext.h
        Compiler.c
        Compiler.h
        Compiler.y
        CompilerFunctions.c
        CompilerFunctions.h
        CompilerTypes.h
        ExpressionTree.h
        Scanner.c
        Scanner.h
        Scanner.l
        TreeFunctions.c
        TreeFunctions.h
        main.cpp
        )

target_include_directories(SimpleCompiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(SimpleCompiler
        m
        )