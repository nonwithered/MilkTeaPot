project(milkpowder)

set(SOURCE_NAMES
    "lib.cc"
    "parse.cc"
    "dump.cc"
)

set(SOURCE_DIR
    "src/main/cpp"
)

foreach(it ${SOURCE_NAMES})
    list(APPEND SOURCE_FILES "${SOURCE_DIR}/${it}")
endforeach(it)

add_library(${PROJECT_NAME}_static STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}_static
    PUBLIC "include"
)

# add_library(${PROJECT_NAME}_shared SHARED ${SOURCE_FILES})

# target_include_directories(${PROJECT_NAME}_shared
#     PUBLIC "include"
# )
