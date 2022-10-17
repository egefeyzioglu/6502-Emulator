cmake_minimum_required(VERSION 3.5)

project(6502Emulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/Qt/6.3.1/mingw_64/lib/cmake")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/windows/icon.rc)
endif()

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/mos6502.cpp
        src/mos6502.h
        src/emulator.h
        src/emulator.cpp
        src/resources.qrc
        src/log.h
        src/log.cpp
        src/loadedfile.h
        src/syntaxhighlighter.h
        src/syntaxhighlighter.cpp
        src/memorymodel.h
        src/memorymodel.cpp
        src/memorymappeddevice.h
        src/programram.h
        src/programram.cpp
        src/rom.h
        src/rom.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(6502Emulator WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${WIN32_RESOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET 6502Emulator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(6502Emulator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(6502Emulator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(6502Emulator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(6502Emulator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS 6502Emulator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(6502Emulator)
endif()
