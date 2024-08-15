

set(libname alumilib)
add_library(${libname} STATIC)

target_include_directories(${libname} PUBLIC include)
target_sources(${libname} 
PRIVATE
	"include/alumi/lexer/lexer_detail.h"
    "include/alumi/lexer/alumi_lexicon.h"
    "include/alumi/lexer/token.h" 
    "include/alumi/lexer/lexed_text.h"
    "include/alumi/lexer.h" 
	"include/alumi/parser/data.h"
      
    "include/alumi/parser/grammar/rule_macro.h" 
    "include/alumi/syntax_tree/walker.h" 
    "include/alumi/syntax_tree/node_children.h" 
    "include/alumi/syntax_tree/node_types.h"
    "include/alumi/syntax_tree/tree_utiltiy_ops.h" 
    "include/alumi/syntax_tree.h" 
    "include/alumi/text_pos.h"
    "include/alumi/error_codes.h" 
    "include/alumi/module.h" 
    "include/alumi/syntax_tree/node_utils.h" 
 
	"source/alumi/lexer.cpp"
	"source/alumi/parser.cpp"
	"source/alumi/text_pos.cpp"
    "source/alumi/lexer/token.cpp"
	"source/alumi/lexer/lexer_detail.cpp"
    "source/alumi/lexer/lexed_text.cpp" 
    "source/alumi/parser/data.cpp"
    	
	"source/alumi/syntax_tree/walker.cpp"  
    "source/alumi/syntax_tree/node_types.cpp"
    "source/alumi/syntax_tree/tree_utiltiy_ops.cpp"  
    "source/alumi/syntax_tree/node_utils.cpp"
    "source/alumi/syntax_tree.cpp" 
    "source/alumi/syntax_tree/node_children.cpp" 

    "modules/syntax_tree/nodes/types.cpp" 
)

target_sources(${libname}
  PUBLIC
    FILE_SET ${libname}_modules TYPE CXX_MODULES FILES
    "modules/parser/grammar/function_parameter.ixx" 
    "modules/parser/grammar/function_declaration.ixx" 
    "modules/parser/grammar/function_definition.ixx" 
    "modules/parser/grammar/expression.ixx" 
    "modules/parser/grammar/assignment.ixx" 
    "modules/parser/grammar/code_block.ixx" 
    "modules/parser/grammar/statement.ixx"   
    "modules/parser/grammar/grammar.ixx"
    "modules/parser/grammar/root.ixx" 
    "modules/parser/grammar/value.ixx" 
    "modules/parser/grammar/tuple.ixx" 
    "modules/parser/grammar/function_call.ixx" 
    "modules/parser/result.ixx" 
    "modules/parser/subparser.ixx" 
    "modules/parser/combinator/combinator.ixx" 
    "modules/parser/combinator/peek.ixx" 
    "modules/parser/combinator/concepts.ixx" 
    "modules/parser/combinator/is.ixx" 
    "modules/parser/combinator/swallow.ixx" 
    "modules/parser/combinator/optional.ixx" 
    "modules/parser/combinator/repeats.ixx" 
    "modules/parser/combinator/repeats_with_separator.ixx" 
    "modules/parser/combinator/sequence.ixx"
    "modules/parser/combinator/any_of.ixx" 
    "modules/parser/combinator/rule.ixx"  
    "modules/parser/error_codes.ixx" 
    "modules/parser/combinator/indented.ixx" 
    "modules/parser/combinator/dedented.ixx" 
    "modules/parser/combinator/no_indent.ixx" 
    "modules/parser/parser.ixx"

    "modules/syntax_tree/node_builder/nodes_builder.ixx" 
    "modules/syntax_tree/nodes/types.ixx" 
    "modules/syntax_tree/nodes/operations.ixx"
    "modules/syntax_tree/nodes/node.ixx"
    "modules/syntax_tree/nodes/nodes.ixx"         
    "modules/syntax_tree/syntax_tree.ixx"
)

find_package(utf8cpp)
find_package(fmt)

target_link_libraries(${libname}  PUBLIC fmt::fmt utf8cpp)

add_subdirectory(test)