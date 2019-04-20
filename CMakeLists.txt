find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)
find_package(Qt5Sql CONFIG REQUIRED)
find_package(Qt5Charts CONFIG REQUIRED)

set(HEADERS
    mainwindow.h
    wizard/importwizard.h
    wizard/intropage.h
    wizard/databasepage.h
    wizard/importtypepage.h
    wizard/localimportpage.h
    wizard/conclusionpage.h
    wizard/remoteimportoverviewpage.h
    wizard/tablespage.h
    threads/tablecreatorthread.h
    threads/remotescannerthread.h
    wizard/remoteimportscanpage.h
    common.h
    threads/importthread.h
    wizard/importprogresspage.h
    dialogs/opendialog.h
    dialogs/graphdialog.h
    models/logmodel.h
    threads/projectopenerthread.h
)

set(SOURCES
    main.cpp
    mainwindow.cpp
    wizard/importwizard.cpp
    wizard/intropage.cpp
    wizard/databasepage.cpp
    wizard/importtypepage.cpp
    wizard/localimportpage.cpp
    wizard/conclusionpage.cpp
    wizard/remoteimportoverviewpage.cpp
    wizard/tablespage.cpp
    threads/tablecreatorthread.cpp
    threads/remotescannerthread.cpp
    wizard/remoteimportscanpage.cpp
    threads/importthread.cpp
    wizard/importprogresspage.cpp
    dialogs/opendialog.cpp
    dialogs/graphdialog.cpp
    models/logmodel.cpp
    threads/projectopenerthread.cpp
)

set(FORMS
    mainwindow.ui
    dialogs/opendialog.ui
    wizard/intropage.ui
    wizard/databasepage.ui
    dialogs/graphdialog.ui
    wizard/tablespage.ui
    wizard/importtypepage.ui
    wizard/localimportpage.ui
)

set(RESOURCES
    loganalyzer_resources.qrc
)

add_executable(loganalyzer ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

target_link_libraries(loganalyzer stdc++ Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Sql Qt5::Charts dbcorelib dbguilib)
