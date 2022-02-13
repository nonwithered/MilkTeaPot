project(milk_windows)

set(src_files
    "main.cc"
    "proxy.cc"
)

set(src_dir
    "src/main/cpp"
)

foreach(it ${src_files})
    list(APPEND src_paths "${src_dir}/${it}")
endforeach(it)

add_executable(${PROJECT_NAME} ${src_paths})

target_link_libraries(${PROJECT_NAME}
    PRIVATE milk_console_static
    PRIVATE soybean_windows_static
)
