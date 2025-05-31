QT += core gui network widgets charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

CONFIG += c++17

TARGET = WeatherCalendar
TEMPLATE = app

SOURCES += \
    calendardelegate.cpp \
    calendarmodel.cpp \
    calendarview.cpp \
    holidaymanager.cpp \
    icalparser.cpp \
    lunarcalculator.cpp \
    main.cpp \
    weathercalendar.cpp

HEADERS += \
    calendardelegate.h \
    calendarmodel.h \
    calendarview.h \
    holidaymanager.h \
    icalparser.h \
    lunarcalculator.h \
    weathercalendar.h

RESOURCES += holidays.json \
    resources.qrc

FORMS +=

