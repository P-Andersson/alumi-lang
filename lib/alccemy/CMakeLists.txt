set(libname alccemy)
add_library(${libname} STATIC)

target_include_directories(${libname} PUBLIC include)
#target_sources(${libname} 
#PRIVATE
#    "modules/syntax_tree/nodes/types.cpp" 
#)

target_sources(${libname}
  PUBLIC FILE_SET ${libname}_modules TYPE CXX_MODULES FILES
    "modules/alccemy.ixx"
    "modules/lexer/lexer.ixx"
    "modules/lexer/patterns.ixx" 
    "modules/lexer/text.ixx"
    "modules/lexer/token.ixx"
    "modules/lexer/tokenized_text.ixx"
    "modules/lexer/unicode.ixx"
)

find_package(utf8cpp)
target_link_libraries(${libname}  PUBLIC utf8cpp)

add_subdirectory(test)