QT += core gui network widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

CONFIG += c++17

TARGET = WeatherCalendar
TEMPLATE = app

SOURCES += \
    calendardelegate.cpp \
    calendarmodel.cpp \
    calendarview.cpp \
    holidaymanager.cpp \
    lunarcalculator.cpp \
    main.cpp \
    scheduleitem.cpp \
    weathercalendar.cpp

HEADERS += \
    calendardelegate.h \
    calendarmodel.h \
    calendarview.h \
    holidaymanager.h \
    lunarcalculator.h \
    scheduleitem.h \
    weathercalendar.h

RESOURCES += holidays.json \
    resources.qrc

LIBS += -lz
