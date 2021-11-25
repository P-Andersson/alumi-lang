set(tname alumilib_tests)

add_executable(${tname})

target_include_directories(${tname} PUBLIC include PRIVATE src)

target_sources(${tname}
               PRIVATE
                  src/main.cpp
                  "src/alumi/lexer/test_lexer_detail.cpp"
                  "src/alumi/lexer/test_lexicon.cpp" 
                  "src/alumi/lexer/test_lexer.cpp" 
                  "src/alumi/parser/test_parser_parts.cpp"
                  "src/alumi/parser/test_parse_rule.cpp"
                  "src/alumi/parser/test_parser.cpp" 
                  "src/syntax_tree/test_nodes.cpp" 
                  "src/syntax_tree/test_tree_nodes.cpp")

find_package(Catch2)

target_link_libraries(${tname} PRIVATE alumilib Catch2::Catch2)

add_test(NAME ${tname} COMMAND ${tname})

add_custom_command(TARGET ${tname}
                   POST_BUILD
                   COMMAND ctest ${tname} -C $<CONFIGURATION> --output-on-failure)