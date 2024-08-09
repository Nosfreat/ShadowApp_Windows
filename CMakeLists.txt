cmake_minimum_required(VERSION 3.5)

project(ShadowsApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find OpenCV components
# find_package(OpenCV REQUIRED)

# Include Eigen directory
set(EIGEN3_INCLUDE_DIR "E:/AGH/Magisterka/Build/eigen-3.4.0")
include_directories(${EIGEN3_INCLUDE_DIR})

# Include the OpenCV directories
set(OpenCV_DIR "C:/OpenCV/install") # Path to the OpenCV installation
find_package(OpenCV REQUIRED) # Find OpenCV package
include_directories(${OpenCV_INCLUDE_DIRS})

# Debug messages
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ShadowsApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        functions.cpp helpdialog.cpp thien_lin_shadows.cpp
        functions.h helpdialog.h thien_lin_shadows.h
        helpdialog.ui img_sources.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ShadowsApp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ShadowsApp SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ShadowsApp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ShadowsApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} opencv_core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ShadowsApp)
endif()
set_target_properties(ShadowsApp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ShadowsApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ShadowsApp)
endif()
