#-------------------------------------------------
#
# Project created by QtCreator 2022-07-10T01:16:07
#
#-------------------------------------------------

QT       += core gui opengl charts

LIBS += -lopengl32
LIBS += -lglu32
LIBS += -lglut

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoralRugosity_Zhihao
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    utility/tinyply.cpp \
    glwidget.cpp \
    rugosityalgorithm.cpp \
    object_Mesh.cpp \
    object_Octree.cpp

HEADERS  += mainwindow.h \
    utility/happly.h \
    utility/kdtree.h \
    utility/tinyply.h \
    utility/utils.h \
    glwidget.h \
    object.h \
    rugosityalgorithm.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
