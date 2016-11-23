#-------------------------------------------------
#
# Project created by QtCreator 2015-10-21T14:49:31
#
#-------------------------------------------------

QT                 += core gui
QT                 += testlib
CONFIG             += c++11
CONFIG             += console

QMAKE_MAC_SDK = macosx10.11



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = TeamPeach
TEMPLATE = app

DISTFILES += \
    TeamPeach.pro.user \
    testing/tc_4.1

HEADERS += \
    gui/CustomSort.h \
    gui/ErrorEditDialog.h \
    gui/piechartwidget.h \
    gui/mainwindow.h \
    database/CSVReader.h \
    database/RecordsManager.h \
    database/QSortListIO.h\
    datamodel/TreeItem.h \
    datamodel/TreeModel.h \
    datamodel/PresentationTreeModel.h \
    datamodel/PublicationTreeModel.h \
    datamodel/GrantFundingTreeModel.h \
    datamodel/TeachingTreeModel.h \
    database/TestCSVReader.h \
    gui/qcustomplot.h \
    testing/test.h \
    testing/test_1.1.h \
    testing/test_1.3.h \
    testing/test_3.1.h \
    testing/test_2.1.h \
    testing/test_4.1.h \
    gui/homepage.h \
    database/CSVWriter.h \
    testing/tc_8_1.h

SOURCES += \
    gui/CustomSort.cpp \
    gui/ErrorEditDialog.cpp \
    gui/piechartwidget.cpp \
    gui/main.cpp \
    gui/mainwindow.cpp \
    database/CSVReader.cpp \
    database/RecordsManager.cpp \
    database/QSortListIO.cpp \
    datamodel/TreeItem.cpp \
    datamodel/TreeModel.cpp \
    datamodel/PresentationTreeModel.cpp \
    datamodel/PublicationTreeModel.cpp \
    datamodel/GrantFundingTreeModel.cpp \
    datamodel/TeachingTreeModel.cpp \
    database/TestCSVReader.cpp \
    gui/qcustomplot.cpp \
    testing/test.cpp \
    testing/tc_1.1.cpp \
    testing/tc_1.3.cpp \
    testing/test_3.1.cpp \
    testing/test_2.1.cpp \
    testing/tc_4.1.cpp \
    gui/homepage.cpp \
    database/CSVWriter.cpp \
    testing/tc_8_1.cpp

FORMS += \
    gui/customsort.ui \
    gui/mainwindow.ui \
    gui/ErrorEditDialog.ui \
    gui/homepage.ui

RESOURCES += \
    images.qrc
