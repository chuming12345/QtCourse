QT       += core gui widgets sql

CONFIG   += c++11

TARGET   = HospitalMS
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    patientdelegate.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    patientdelegate.h
