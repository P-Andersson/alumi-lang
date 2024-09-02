set(libname alccemy)
add_library(${libname} STATIC)

target_include_directories(${libname} PUBLIC include)

target_sources(${libname}
  PUBLIC FILE_SET ${libname}_modules TYPE CXX_MODULES FILES
    "modules/alccemy.ixx"
    
    "modules/lexer/concepts.ixx"
    "modules/lexer/errors.ixx"
    "modules/lexer/lexer.ixx" 
    "modules/lexer/patterns.ixx" 
    "modules/lexer/text.ixx"
    "modules/lexer/token.ixx"
    "modules/lexer/tokenized_text.ixx"
    "modules/lexer/unicode.ixx"
   
    "modules/lexer/rules/concepts.ixx"
    "modules/lexer/rules/indention.ixx"
    "modules/lexer/rules/rules.ixx"
    "modules/lexer/rules/types.ixx"
    
    "modules/util/tuple.ixx"
    "modules/util/unique_type_args.ixx"
    "modules/util/variant.ixx"
)

find_package(fmt)
find_package(utf8cpp)

target_link_libraries(${libname}  PUBLIC fmt::fmt utf8cpp)

add_subdirectory(test)

add_subdirectory(examples)