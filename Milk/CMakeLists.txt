project(milk)

set(SOURCE_NAMES
    "main.cc"
)

set(SOURCE_DIR
    "src/main/cpp"
)

foreach(it ${SOURCE_NAMES})
    list(APPEND SOURCE_FILES "${SOURCE_DIR}/${it}")
endforeach(it)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE milkpowder_static
)
