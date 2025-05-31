#include <QApplication>
#include "weathercalendar.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WeatherCalendar w;
    w.show();
    return a.exec();
}
