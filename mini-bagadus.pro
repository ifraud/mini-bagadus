QT += core
QT -= gui

TARGET = mini-bagadus
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += "C:\Program Files\IDS\uEye\Develop\include"

TEMPLATE = app

SOURCES += main.cpp

LIBS += "C:\Program Files\IDS\uEye\Develop\Lib\uEye_api_64.lib"
