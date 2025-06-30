#include "calendarmodel.h"
#include<QBrush>

CalendarModel::CalendarModel(QObject *parent)
    : QAbstractTableModel(parent), m_currentDate(QDate::currentDate()) {
    m_holidayMgr = new HolidayManager(this);
    m_holidayMgr->loadFromFile(":/data/holidays.json");
}

int CalendarModel::rowCount(const QModelIndex &) const {
    return 6;  // 固定6行
}

int CalendarModel::columnCount(const QModelIndex &) const {
    return 7;  // 固定7列（周日至周六）
}

// calendarmodel.cpp
// calendarmodel.cpp
QVariant CalendarModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    QDate date = dateFromIndex(index);

    if (role == Qt::BackgroundRole) {
        if (m_highlightedDates.contains(date)) {
            return QBrush(QColor(255, 200, 200)); // 粉色背景高亮
        }
    }

    switch (role) {
    case Qt::DisplayRole:
        return ""; // 清空默认显示
    case DateRole:
        return date;
    case LunarRole: {
        LunarCalculator::LunarDate lunar = m_lunarCalc.convertSolarToLunar(date);
        return lunar.displayText();
    }
    case HolidayRole: {
        QString holiday = m_holidayMgr->getHoliday(date);
        return holiday;
    }
    case ScheduleCountRole: {
        if (!m_schedules) return 0; // 安全检查
        QDate date = dateFromIndex(index);
        int count = m_schedules->value(date).size();
        return qMin(count, 10);
    }
    default:
        return QVariant();
    }
}

QVariant CalendarModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return QLocale().dayName(section + 1, QLocale::ShortFormat);
    }
    return QVariant();
}

void CalendarModel::setCurrentDate(const QDate &date) {
    beginResetModel();
    m_currentDate = date;
    endResetModel();
}

QDate CalendarModel::dateFromIndex(const QModelIndex &index) const
{
    // 以当月第一天为基准计算
    QDate firstDay = m_currentDate.addDays(1 - m_currentDate.day());
    int weekDay = firstDay.dayOfWeek();

    // 计算正确的日期偏移
    int offset = index.row() * 7 + index.column() - weekDay + 1;
    return firstDay.addDays(offset);
}

void CalendarModel::setSchedules(const QMap<QDate, QList<ScheduleItem>>* schedules) {
    m_schedules = schedules;
}

const QMap<QDate, QList<ScheduleItem>>* CalendarModel::schedules() const {
    return m_schedules;
}
