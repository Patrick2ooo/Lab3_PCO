TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++17

LIBS += -lgtest -lpcosynchro -lpthread

SOURCES += \
        main.cpp
