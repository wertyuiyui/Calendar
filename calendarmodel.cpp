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
void CalendarModel::setSchedules(const QMap<QDate, QString> &schedules) {
    beginResetModel();
    m_schedules = schedules;
    endResetModel();
}
// calendarmodel.cpp
QVariant CalendarModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    QDate date = dateFromIndex(index);
    qDebug() << "[模型查询] 日期：" << date << "角色：" << role;

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
        qDebug() << "获取节日：" << date.toString("MMdd") << "=>" << holiday;
        return holiday;
    }
    case ScheduleRole:
        return m_schedules.value(date, "");
    default:
        return QVariant();
    }
    if (date == QDate::currentDate()) {
        return QBrush(Qt::yellow);
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

