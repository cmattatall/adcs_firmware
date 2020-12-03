cmake_minimum_required(VERSION 3.16)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Positive Json parsing test (expect all JSONs to pass)
    add_executable(positiveJsonParsingTest)
    target_sources(positiveJsonParsingTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/positiveParseTest.c")
    target_link_libraries(positiveJsonParsingTest PUBLIC JTOK)
    add_test(NAME positiveJsonParsingTest 
            COMMAND positiveJsonParsingTest
            --build-generator "${CMAKE_GENERATOR}"
            --test-command "${CMAKE_CTEST_COMMAND}"
    )

    # Negative Json parsing test (expect all "JSONS string (they are invalid)"
    add_executable(negativeJsonParsingTest)
    target_sources(negativeJsonParsingTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/negativeParseTest.c")
    target_link_libraries(negativeJsonParsingTest PUBLIC JTOK)
    add_test( NAME negativeJsonParsingTest
              COMMAND negativeJsonParsingTest
              --build-generator "${CMAKE_GENERATOR}"
              --test-command "${CMAKE_CTEST_COMMAND}"       
    )
endif()






