TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++17

LIBS += -lgtest -lpcosynchro -lpthread

SOURCES += \
        main.cpp

# Copy the python script to let it being used by the application
QMAKE_POST_LINK = $$QMAKE_COPY $$PWD/show_graphic_from_exec.py $${OUT_PWD}

