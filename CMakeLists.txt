cmake_minimum_required(VERSION 3.16)
project(lgl-emoji-picker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_executable(lgl-emoji-picker
    src/main.cpp
    src/emojipicker.cpp
    src/emojipicker.h
    src/recentemojis.cpp
    src/recentemojis.h
    src/emojidata.h
    resources.qrc
)

target_link_libraries(lgl-emoji-picker PRIVATE Qt6::Core Qt6::Widgets)

target_compile_options(lgl-emoji-picker PRIVATE
    -Wall -Wextra -Wpedantic
)

install(TARGETS lgl-emoji-picker RUNTIME DESTINATION bin)
install(FILES packaging/lgl-emoji-picker.desktop
    DESTINATION share/applications)
install(FILES packaging/lgl-emoji-picker.metainfo.xml
    DESTINATION share/metainfo)
