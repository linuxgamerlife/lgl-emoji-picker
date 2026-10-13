cmake_minimum_required(VERSION 3.20)

# LGL Emoji Picker, GTK 4 (gtkmm) application. This is the build that is packaged; the Qt
# sources at the repository root are kept for reference only.
project(lgl-emoji-picker VERSION 1.0.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CTest)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall> $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
                        $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic> $<$<COMPILE_LANGUAGE:CXX>:-Werror>)
endif()

option(EMOJIPICKER_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(EMOJIPICKER_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0>=4.20)
pkg_check_modules(ADWAITA REQUIRED IMPORTED_TARGET libadwaita-1>=1.6)
find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

set(EMOJIPICKER_REPO_ROOT ${PROJECT_SOURCE_DIR}/..)

# --- resources -----------------------------------------------------------------

set(RESOURCE_XML ${PROJECT_SOURCE_DIR}/data/emojipicker.gresource.xml)
set(RESOURCE_C ${CMAKE_CURRENT_BINARY_DIR}/emojipicker-resources.c)
set(RESOURCE_DEPS
    ${PROJECT_SOURCE_DIR}/data/style.css
    ${EMOJIPICKER_REPO_ROOT}/references/Black-Don.png)
add_custom_command(
    OUTPUT ${RESOURCE_C}
    COMMAND ${GLIB_COMPILE_RESOURCES} --generate-source --target=${RESOURCE_C}
            --sourcedir=${PROJECT_SOURCE_DIR}/data --sourcedir=${EMOJIPICKER_REPO_ROOT}
            ${RESOURCE_XML}
    DEPENDS ${RESOURCE_XML} ${RESOURCE_DEPS}
    VERBATIM)
set_source_files_properties(${RESOURCE_C} PROPERTIES COMPILE_OPTIONS "-w")

# --- core (no widgets; testable) --------------------------------------------------

add_library(emojipicker_core STATIC
    src/core/clipboardhelper.cpp
    src/core/clipboardhelper.h
    src/core/emojidata.h
    src/core/recentemojis.cpp
    src/core/recentemojis.h
    src/core/search.cpp
    src/core/search.h
    src/core/theme.cpp
    src/core/theme.h
)
target_include_directories(emojipicker_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(emojipicker_core PUBLIC PkgConfig::GTKMM)

# --- application -----------------------------------------------------------------

add_executable(lgl-emoji-picker
    ${RESOURCE_C}
    src/main.cpp
    src/ui/themewatcher.cpp
    src/ui/themewatcher.h
    src/ui/window.cpp
    src/ui/window.h
)
target_link_libraries(lgl-emoji-picker PRIVATE emojipicker_core PkgConfig::ADWAITA)

install(TARGETS lgl-emoji-picker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${EMOJIPICKER_REPO_ROOT}/packaging/lgl-emoji-picker.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ${EMOJIPICKER_REPO_ROOT}/packaging/lgl-emoji-picker.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
foreach(size 48 64 128 256)
    install(FILES ${EMOJIPICKER_REPO_ROOT}/packaging/icons/${size}x${size}/lgl-emoji-picker.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${size}x${size}/apps)
endforeach()

# --- tests -----------------------------------------------------------------------

if(BUILD_TESTING)
    foreach(test clipboard recent search theme)
        add_executable(${test}_test tests/${test}_test.cpp)
        target_link_libraries(${test}_test PRIVATE emojipicker_core)
        add_test(NAME ${test} COMMAND ${test}_test)
    endforeach()
    # g_test_skip() reports 77 under the TAP harness.
    set_tests_properties(clipboard recent search theme PROPERTIES SKIP_RETURN_CODE 77)
endif()
