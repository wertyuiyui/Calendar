#ifndef ICALPARSER_H
#define ICALPARSER_H
#pragma once
#include <QObject>
#include <QMap>
#include <QDate>

class IcalParser : public QObject {
    Q_OBJECT
public:
    explicit IcalParser(QObject *parent = nullptr);
    QMap<QDate, QString> parseEvents(const QByteArray &data);

private:
    QDate parseIcalDate(const QString &dateStr);
};
#endif // ICALPARSER_H
