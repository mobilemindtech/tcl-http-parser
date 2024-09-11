project(HttpParserTests LANGUAGES C)

add_executable(HttpParserTests src/test.c)
target_link_libraries(HttpParserTests PRIVATE HttpParser)


add_test(
  NAME HttpParserTests
  COMMAND $<TARGET_FILE:HttpParserTests>
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
    --force-new-ctest-process
    --verbose
    --output-on-failure
)
