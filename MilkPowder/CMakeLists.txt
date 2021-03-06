project(milkpowder)

set(src_files
    "lib.cc"
    "parse.cc"
    "dump.cc"
)

set(src_dir
    "src/main/cpp/"
)

set(api_dir
    "include/"
)

foreach(it ${src_files})
    list(APPEND src_paths "${src_dir}${it}")
endforeach(it)

add_library(${PROJECT_NAME}_static STATIC ${src_paths})

target_include_directories(${PROJECT_NAME}_static
    PUBLIC ${api_dir}
)

target_link_libraries(${PROJECT_NAME}_static
    PUBLIC tea_static
    PRIVATE milktea_static
)

add_library(${PROJECT_NAME}_shared SHARED ${src_paths})

target_include_directories(${PROJECT_NAME}_shared
    PUBLIC ${api_dir}
)

target_link_libraries(${PROJECT_NAME}_shared
    PUBLIC tea_shared
    PRIVATE milktea_shared
)
