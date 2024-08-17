set(tname alccemy_test)

add_executable(${tname})

target_include_directories(${tname} PUBLIC include PRIVATE src)

target_sources(${tname}
               PRIVATE
                 "src/lexer/test_patterns.cpp" 
                 "src/lexer/test_lexer.cpp"
)

find_package(Catch2)

target_link_libraries(${tname} PRIVATE alccemy Catch2::Catch2WithMain)

add_test(NAME ${tname} COMMAND ${tname})

#add_custom_command(TARGET ${tname}
#                   POST_BUILD
#                   COMMAND ctest ${tname} -C $<CONFIGURATION> --output-on-failure)