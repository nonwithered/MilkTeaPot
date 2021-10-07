project(milk)

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
    PRIVATE milkpowder_static
    PRIVATE soybean_static
    PRIVATE soymilk_static
)

message("${PROJECT_NAME}: MilkTeaPot_system_flag_linux = ${MilkTeaPot_system_flag_linux}")
if (MilkTeaPot_system_flag_linux MATCHES "ON")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE soybean_linux_static
    )
endif()

message("${PROJECT_NAME}: MilkTeaPot_system_flag_windows = ${MilkTeaPot_system_flag_windows}")
if (MilkTeaPot_system_flag_windows MATCHES "ON")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE soybean_windows_static
    )
endif()

message("${PROJECT_NAME}: MilkTeaPot_system_flag_mac = ${MilkTeaPot_system_flag_mac}")
if (MilkTeaPot_system_flag_mac MATCHES "ON")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE soybean_mac_static
    )
endif()
