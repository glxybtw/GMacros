cmake_minimum_required(VERSION 3.16)
project(GMacros LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)


find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)


get_target_property(MOC_EXECUTABLE Qt6::moc IMPORTED_LOCATION)

add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/main.moc"
    COMMAND "${MOC_EXECUTABLE}"
            "$<$<BOOL:${Qt6Core_VERSION}>:-DQT_CORE_LIB>"
            -I "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
            -o "${CMAKE_CURRENT_BINARY_DIR}/main.moc"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    COMMENT "Running moc on main.cpp"
    VERBATIM
)


add_custom_target(generate_main_moc
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/main.moc"
)


set(SOURCES
    main.cpp

    "${CMAKE_CURRENT_BINARY_DIR}/main.moc"

    "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.o"
)


set(RESOURCES resources.qrc)


if(WIN32)
    set(WIN32_FLAG WIN32)

endif()


qt_add_executable(GMacros
    ${WIN32_FLAG}
    ${SOURCES}
    ${RESOURCES}
)


add_dependencies(GMacros generate_main_moc)


target_include_directories(GMacros PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")


target_link_libraries(GMacros PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)


if(WIN32)
    target_link_libraries(GMacros PRIVATE user32 dwmapi)
endif()


if(WIN32)
    add_custom_command(TARGET GMacros POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe"
                --no-translations
                "$<TARGET_FILE:GMacros>"
        COMMENT "Running windeployqt..."
        VERBATIM
    )
endif()