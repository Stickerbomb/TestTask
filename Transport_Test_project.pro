QT += widgets
QT += xml
requires(qtConfig(treeview))

FORMS       = mainwindow.ui
HEADERS     = mainwindow.h \
              treeitem.h \
              treemodel.h \
              typefile.h \
              xmlparser.h
RESOURCES   = Transport_Test_project.qrc
SOURCES     = mainwindow.cpp \
              treeitem.cpp \
              treemodel.cpp \
              main.cpp \
              typefile.cpp \
              xmlparser.cpp

