set(binname arintegra)
add_executable(${binname})

target_include_directories(${binname} PUBLIC include)

target_sources(${binname}
PRIVATE
    "src/main.cpp"
)

target_link_libraries(${binname}  PUBLIC alccemy)
