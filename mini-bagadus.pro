QT += core
QT -= gui

TARGET = mini-bagadus
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += "C:\Program Files\IDS\uEye\Develop\include" "C:\opencv\opencv\build\include"


TEMPLATE = app

SOURCES += main.cpp

LIBS += "C:\Program Files\IDS\uEye\Develop\Lib\uEye_api_64.lib"
#LIBS += "C:\opencv\opencv\build\lib\Release\opencv_core300.lib"
#LIBS += "C:\opencv\opencv\build\lib\Release\opencv_imgproc300.lib"
#LIBS += "C:\opencv\opencv\build\lib\Release\opencv_highgui300.lib"
#LIBS += "C:\opencv\opencv\build\lib\Release\opencv_videoio300.lib"
#LIBS += "C:\opencv\opencv\build\lib\Release\opencv_imgcodecs300.lib"
#LIBS += -L"C:\opencv\opencv\build\bin\Release"
#LIBS += -L"C:\opencv\opencv\build\lib\Release"
#LIBS += -lopencv_imgproc300 -lopencv_core300 -lopencv_highgui300 -lopencv_videoio300 -lopencv_imgcodecs300
#LIBS += -L"C:\Users\vamsidhar\Documents\ffmpeg-20151113-git-6f520ce-win64-dev\lib"
#LIBS += -lavcodec -lavformat -lswscale -lavdevice -lpostproc -lavutil -lswresample
