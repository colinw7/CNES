TEMPLATE = lib

TARGET = CQNES

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \

MOC_DIR = .moc

CONFIG += staticlib
CONFIG += c++14

SOURCES += \
CQNES_Cartridge.cpp \
CQNES_CPU.cpp \
CQNES_Machine.cpp \
CQNES_PPU.cpp \
CQNES_Sprites.cpp \

HEADERS += \
../qinclude/CQNES_Cartridge.h \
../qinclude/CQNES_CPU.h \
../qinclude/CQNES_Machine.h \
../qinclude/CQNES_PPU.h \
../qinclude/CQNES_Sprites.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../qinclude \
../include \
../../C6502/qinclude \
../../C6502/include \
../../CStrUtil/include \
