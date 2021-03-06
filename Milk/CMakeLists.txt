project(milk)

set(src_files
    "lib.cc"
    "context.cc"
)

set(src_dir
    "src/main/cpp"
)

set(api_dir
    "include/"
)

foreach(it ${src_files})
    list(APPEND src_paths "${src_dir}/${it}")
endforeach(it)

add_library(${PROJECT_NAME}_static STATIC ${src_paths})

target_include_directories(${PROJECT_NAME}_static
    PUBLIC ${api_dir}
)

target_link_libraries(${PROJECT_NAME}_static
    PUBLIC milktea_command_static
    PUBLIC milktea_static
    PUBLIC milkpowder_static
    PUBLIC soymilk_static
)

add_library(${PROJECT_NAME}_shared SHARED ${src_paths})

target_include_directories(${PROJECT_NAME}_shared
    PUBLIC ${api_dir}
)

target_link_libraries(${PROJECT_NAME}_shared
    PUBLIC milktea_command_shared
    PUBLIC milktea_shared
    PUBLIC milkpowder_shared
    PUBLIC soymilk_shared
)
