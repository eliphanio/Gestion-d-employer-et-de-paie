cmake_minimum_required(VERSION 3.16)

project(GestionEmployer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Charts LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Charts LinguistTools)

# Emplacement des fichiers Qt Designer
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# Fichier de traduction
set(TS_FILES
    translations/GestionEmployer_fr_FR.ts
)

set(TS_FILES GestionEmployer_fr_FR.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        include/mainwindow.h
        ui/mainwindow.ui
        database/schema.sql
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    qt_add_executable(GestionEmployer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/database.h
        src/database.cpp
        src/schema.cpp
        include/addpostdialog.h src/addpostdialog.cpp ui/addpostdialog.ui
        include/removepostdialog.h src/removepostdialog.cpp ui/removepostdialog.ui
        include/paie_calcul.h src/paie_calcul.cpp
        include/ecrans_paie.h src/ecrans_paie.cpp

    )

target_include_directories(GestionEmployer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GestionEmployer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(GestionEmployer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GestionEmployer
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(GestionEmployer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GestionEmployer)
endif()
set_target_properties(GestionEmployer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GestionEmployer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GestionEmployer)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/database")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/database/schema.sql"
    "${CMAKE_CURRENT_BINARY_DIR}/database/schema.sql"
    COPYONLY
)



project(GestionPaie LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Sql
    PrintSupport
)

qt_add_executable(GestionPaie
    main.cpp
    mainwindow.h
    mainwindow.cpp
    paie_calcul.h
    paie_calcul.cpp
    database.h
    database.cpp
)

target_link_libraries(GestionPaie PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
)

