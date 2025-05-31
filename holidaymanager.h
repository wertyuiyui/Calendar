#ifndef HOLIDAYMANAGER_H
#define HOLIDAYMANAGER_H
#include <QObject>
#include <QMap>

class HolidayManager : public QObject {
    Q_OBJECT
public:
    explicit HolidayManager(QObject *parent = nullptr);
    void loadFromFile(const QString &path);
    QString getHoliday(const QDate &date) const;

private:
    QMap<QString, QString> m_holidays;
};
#endif // HOLIDAYMANAGER_H
