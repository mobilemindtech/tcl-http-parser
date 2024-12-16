project(HttpParserTests LANGUAGES C)

add_executable(HttpParserTests src/test.c)
target_link_libraries(HttpParserTests PRIVATE HttpParser)

target_compile_options(HttpParserTests PRIVATE -Wno-incompatible-pointer-types)

add_test(
  NAME HttpParserTests
  COMMAND $<TARGET_FILE:HttpParserTests>
)

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
#    --force-new-ctest-process
#    --verbose
#    --output-on-failure
#  )

add_custom_target(
  Test ALL
  COMMAND
  ${CMAKE_CTEST_COMMAND}
   --force-new-ctest-process
   --verbose
   --output-on-failure
  COMMENT "Run unit tests")

add_custom_target(
  TclTest ALL
  COMMAND cd ../tests && tclsh all.tcl
  COMMENT "Run TCL tests")


