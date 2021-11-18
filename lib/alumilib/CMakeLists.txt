

set(libname alumilib)
add_library(${libname} STATIC)

target_include_directories(${libname} PUBLIC include)
target_sources(${libname} 
PRIVATE
	"include/alumi/lexer/lexer_detail.h"
    "include/alumi/lexer/alumi_lexicon.h"
    "include/alumi/lexer/token.h" 
    "include/alumi/lexer.h" 
	"include/alumi/parser/data.h"
    "include/alumi/parser/parse_rule.h"
    "include/alumi/parser/parser_parts.h"
    "include/alumi/syntax_tree/modue_tree.h" 
	"include/alumi/text_pos.h"

	"source/alumi/lexer/token.cpp"
	"source/alumi/lexer/lexer_detail.cpp"
    "source/alumi/parser/parser_parts.cpp"    
    "source/alumi/syntax_tree/nodes.cpp"	
	"source/alumi/lexer.cpp"
	"source/alumi/parser.cpp"
	"source/alumi/text_pos.cpp"
 "source/alumi/parser/parse_rule.cpp")

find_package(utf8cpp)
find_package(fmt)

target_link_libraries(${libname}  PUBLIC fmt::fmt utf8cpp::utf8cpp)

add_subdirectory(test)