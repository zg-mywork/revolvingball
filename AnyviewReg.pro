SOURCES += \
    main.cpp \
    registry.cpp \
    mainwindow.cpp \
    nkview.cpp \
    vkview.cpp

QMAKE_CXXFLAGS += -std=c++11
QT += widgets

HEADERS += \
    registry_structure.h \
    registry.h \
    treestructure.h \
    mainwindow.h \
    nkview.h \
    vkview.h

OTHER_FILES +=
