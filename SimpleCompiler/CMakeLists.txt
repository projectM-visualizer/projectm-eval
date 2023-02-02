
add_executable(SimpleCompiler
        FileParser.cpp
        FileParser.hpp
        main.cpp
        )

target_link_libraries(SimpleCompiler
        projectM_EvalLib
        )