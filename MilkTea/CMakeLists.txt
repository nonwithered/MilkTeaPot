project(milktea)

set(src_files
    "lib.cc"
)

set(src_dir
    "src/main/cpp/"
)

foreach(it ${src_files})
    list(APPEND src_paths "${src_dir}${it}")
endforeach(it)

add_library(${PROJECT_NAME}_static STATIC ${src_paths})

target_include_directories(${PROJECT_NAME}_static
    PUBLIC "include"
)

add_library(${PROJECT_NAME}_shared SHARED ${src_paths})

target_include_directories(${PROJECT_NAME}_shared
    PUBLIC "include"
)
