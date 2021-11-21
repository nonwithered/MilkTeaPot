project(milk_linux)

set(src_files
    "main.cc"
)

set(src_dir
    "src/main/cpp"
)

foreach(it ${src_files})
    list(APPEND src_paths "${src_dir}/${it}")
endforeach(it)

add_executable(${PROJECT_NAME} ${src_paths})

target_link_libraries(${PROJECT_NAME}
    PRIVATE milk_static
    PRIVATE soybean_linux_static
)
