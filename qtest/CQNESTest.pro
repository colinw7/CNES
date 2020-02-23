TEMPLATE = app

TARGET = CQNESTest

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \

MOC_DIR = .moc

SOURCES += \
CQNESTest.cpp \

HEADERS += \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../qinclude \
../include \
../../C6502/qinclude \
../../C6502/include \
../../CQUtil/include \

unix:LIBS += \
-L../lib \
-L../../C6502/lib \
-L../../CQUtil/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CUtil/lib \
-L../../CMath/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-lCQNES -lCNES -lC6502 \
-lCQUtil -lCConfig -lCUtil -lCImageLib -lCFont \
-lCMath -lCFileUtil -lCFile -lCStrUtil -lCRegExp -lCOS \
-lpng -ljpeg -ltre
