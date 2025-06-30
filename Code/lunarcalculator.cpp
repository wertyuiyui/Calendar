// lunarcalculator.cpp
#include "lunarcalculator.h"
#include <QMap>

QMap<int, LunarCalculator::LunarYearInfo> LunarCalculator::initLunarData() {
    QMap<int, LunarYearInfo> data;

    data[2025] = LunarYearInfo(
        QDate(2025, 1, 29), // 春节日期（阳历2025年1月29日）
        0,                  // 无闰月
        // 每月天数（正月到腊月，共12个月）
        {30,29,30,29,29,30,29,30,29,30,29,30},
        // 闰月标记（全0表示没有闰月）
        {0,0,0,0,0,0,0,0,0,0,0,0}
        );
    return data;
}

LunarCalculator::LunarDate LunarCalculator::convertSolarToLunar(const QDate &date) {
    static const QMap<int, LunarYearInfo> lunarData = initLunarData();
    LunarDate lunarDate;

    // 查找可能的农历年
    auto it = lunarData.upperBound(date.year());
    if (it != lunarData.begin()) --it;

    // 向前查找春节在日期之前的年份
    while (it != lunarData.begin() && it.value().springDate > date) --it;

    // 检查日期是否在该农历年范围内
    const LunarYearInfo& info = it.value();
    QDate current = info.springDate;
    if (date < current) {
        if (it == lunarData.begin()) return lunarDate; // 超出数据范围
        --it; // 使用前一年的数据
    }

    // 遍历月份计算
    for (int i = 0; i < info.monthDays.size(); ++i) {
        int days = info.monthDays[i];
        QDate end = current.addDays(days - 1);

        if (date >= current && date <= end) {
            lunarDate.year = it.key();
            lunarDate.month = i + 1;
            lunarDate.day = date.toJulianDay() - current.toJulianDay() + 1;
            lunarDate.isLeap = info.leapFlags[i];
            lunarDate.festival = getFestival(lunarDate.month, lunarDate.day);
            return lunarDate;
        }
        current = end.addDays(1);
    }

    return lunarDate; // 未找到（数据不完整时）
}

QString LunarCalculator::getFestival(int month, int day) {
    static const QMap<QPair<int, int>, QString> festivals = {
        {{1, 1},  "春节"},
        {{1, 15}, "元宵节"},
        {{5, 5},  "端午节"},
        {{7, 7},  "七夕"},
        {{8, 15}, "中秋节"},
        {{9, 9},  "重阳节"},
        {{12, 8}, "腊八节"}
    };
    return festivals.value(qMakePair(month, day), "");
}
