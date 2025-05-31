#ifndef LUNARCALCULATOR_H
#define LUNARCALCULATOR_H
// lunarcalculator.h
#include <QDate>
#include <QString>

class LunarCalculator {
public:
    struct LunarDate {
        int year;
        int month;
        int day;
        bool isLeap;
        QString monthName; // 农历月份名称（如"正月"）
        QString dayName;
        QString festival;
        QString displayText() const {
            if (day == 1) return monthName; // 月初显示月份
            return dayName;                 // 其他日期显示日名
        }

        LunarDate() : year(0), month(0), day(0), isLeap(false) {}
    };

    static LunarDate convertSolarToLunar(const QDate &date);

private:

    struct LunarYearInfo {
        QDate springDate;  // 春节阳历日期
        int leapMonth;     // 闰月（0表示无）
        QVector<int> monthDays;   // 每月天数
        QVector<bool> leapFlags;  // 是否闰月标记

        LunarYearInfo() : leapMonth(0) {}
        LunarYearInfo(QDate s, int lm, QVector<int> md, QVector<bool> lf)
            : springDate(s), leapMonth(lm), monthDays(md), leapFlags(lf) {}
    };

    static QMap<int, LunarYearInfo> initLunarData();
    static QString getFestival(int month, int day);
};
#endif // LUNARCALCULATOR_H
