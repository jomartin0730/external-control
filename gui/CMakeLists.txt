set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
qt5_add_resources(RESOURCES resources/resources.qrc)
add_executable(app_main
        # ----------------------------------------------------------------------------
        # Headers
        # ----------------------------------------------------------------------------
        mainwindow.h

        # ----------------------------------------------------------------------------
        # Sources (정렬: 알파벳 순)
        # ----------------------------------------------------------------------------
        mainwindow.cpp
        main.cpp

        # -----------------
        # ----------------------------------------------------------------------------
        # UI Files (정렬: 알파벳 순)
        # ----------------------------------------------------------------------------
        mainwindow.ui

        # ----------------------------------------------------------------------------
        # Resources
        # ----------------------------------------------------------------------------
        ${RESOURCES}
)
target_link_libraries(app_main PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        ${PROJECT_NAME}-core
)