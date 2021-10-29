project(milk)

set(src_files
    "lib.cc"
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
    PRIVATE milktea_static
    PRIVATE milkpowder_static
    PRIVATE soybean_static
    PRIVATE soymilk_static
)

add_library(${PROJECT_NAME}_shared SHARED ${src_paths})

target_include_directories(${PROJECT_NAME}_shared
    PUBLIC ${api_dir}
)

target_link_libraries(${PROJECT_NAME}_shared
    PRIVATE milktea_shared
    PRIVATE milkpowder_shared
    PRIVATE soybean_shared
    PRIVATE soymilk_shared
)

if(NOT MilkTeaPot_system_flag MATCHES "OFF")
    if (MilkTeaPot_system_flag_linux MATCHES "ON")
        set(src_main
            "main_linux.cc"
        )
    endif()
    if (MilkTeaPot_system_flag_windows MATCHES "ON")
        set(src_main
            "main_windows.cc"
        )
    endif()
    if (MilkTeaPot_system_flag_mac MATCHES "ON")
        set(src_main
            "main_mac.cc"
        )
    endif()
    add_executable(${PROJECT_NAME} "${src_dir}/${src_main}")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ${PROJECT_NAME}_static
    )
    if (MilkTeaPot_system_flag_linux MATCHES "ON")
        target_link_libraries(${PROJECT_NAME}
            PRIVATE soybean_linux_static
        )
    endif()
    if (MilkTeaPot_system_flag_windows MATCHES "ON")
        target_link_libraries(${PROJECT_NAME}
            PRIVATE soybean_windows_static
        )
    endif()
    if (MilkTeaPot_system_flag_mac MATCHES "ON")
        target_link_libraries(${PROJECT_NAME}
            PRIVATE soybean_mac_static
        )
    endif()
endif()
