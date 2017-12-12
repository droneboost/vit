#-------------------------------------------------
#
# Project created by QtCreator 2017-11-29T15:45:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualInspectTool
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
           mdigvchild.cpp \
    vitiffhandler.cpp

HEADERS  += mainwindow.h \
            mdigvchild.h \
    vitiffhandler.h

FORMS    +=

RESOURCES += resources.qrc

#LIBS += -L$$PWD/lib/tiff/lib/ -ltiff
#PRE_TARGETDEPS += $$PWD/lib/tiff/lib/libtiff.a

INCLUDEPATH += $$PWD/lib/tiff/include
DEPENDPATH += $$PWD/lib/tiff/include

#INCLUDEPATH += $$PWD/lib/opencv/include
#DEPENDPATH += $$PWD/lib/opencv/x86/mingw

#unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_dll/x86/mingw/lib/ -llibopencv_core331.dll
#unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_dll/x86/mingw/lib/ -llibopencv_imgcodecs331.dll
#unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_dll/x86/mingw/lib/ -llibopencv_imgproc331.dll
#unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_dll/x86/mingw/lib/ -llibopencv_highgui331.dll

INCLUDEPATH += $$PWD/lib/opencv_lib/include
DEPENDPATH += $$PWD/lib/opencv_lib/x86/mingw/staticlib

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_highgui331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_highgui331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_highgui331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_imgcodecs331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_imgcodecs331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_imgcodecs331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_imgproc331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_imgproc331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_imgproc331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_core331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_core331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_core331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lIlmImf
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/IlmImf.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libIlmImf.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -llibjpeg
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libjpeg.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/liblibjpeg.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -llibpng
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libpng.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/liblibpng.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -llibjasper
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libjasper.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/liblibjasper.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -llibtiff
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libtiff.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/liblibtiff.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lzlib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libtiff.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/liblibtiff.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_flann331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_flann331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_flann331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_ml331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_ml331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_ml331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_photo331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_photo331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_photo331.a

unix:!macx|win32: LIBS += -L$$PWD/lib/opencv_lib/x86/mingw/staticlib/ -lopencv_features2d331
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/opencv_features2d331.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/lib/opencv_lib/x86/mingw/staticlib/libopencv_features2d331.a

LIBS += -lGdi32 -lComdlg32 #used by opencv highgui


