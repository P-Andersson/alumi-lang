

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
    
    
    "include/alumi/syntax_tree/walker.h" 
    "include/alumi/syntax_tree/node_types.h"
    "include/alumi/syntax_tree/tree_utiltiy_ops.h" 
    "include/alumi/text_pos.h"
    "include/alumi/error_codes.h" 
    "include/alumi/module.h" 
    "include/alumi/syntax_tree/node_utils.h" 
 

	"source/alumi/lexer.cpp"
	"source/alumi/parser.cpp"
	"source/alumi/text_pos.cpp"
    "source/alumi/lexer/token.cpp"
	"source/alumi/lexer/lexer_detail.cpp"
    "source/alumi/parser/data.cpp"
    "source/alumi/parser/parse_rule.cpp" 
    "source/alumi/parser/parser_parts.cpp"    
    "source/alumi/syntax_tree/nodes.cpp"	
	"source/alumi/syntax_tree/walker.cpp"  
     
    "source/alumi/syntax_tree/node_types.cpp"
    "source/alumi/syntax_tree/tree_utiltiy_ops.cpp"  
    "source/alumi/syntax_tree/node_utils.cpp"
 "include/alumi/lexer/lexed_text.h" "source/alumi/lexer/lexed_text.cpp" "include/alumi/syntax_tree.h" "source/alumi/syntax_tree.cpp" "include/alumi/syntax_tree/node_children.h" "source/alumi/syntax_tree/node_children.cpp")
find_package(utf8cpp)
find_package(fmt)

target_link_libraries(${libname}  PUBLIC fmt::fmt utf8cpp::utf8cpp)

add_subdirectory(test)