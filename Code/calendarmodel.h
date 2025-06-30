#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H
#pragma once
#include <QAbstractTableModel>
#include <QDate>
#include "lunarcalculator.h"  // 你的农历计算类
#include "holidaymanager.h"   // 你的节假日管理类
#include "scheduleitem.h"
#include<QSet>
class CalendarModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CalendarModel(QObject *parent = nullptr);
    enum CustomRoles {
        DateRole = Qt::UserRole + 1, // 日期原始数据
        LunarRole,                   // 农历信息
        HolidayRole,                  // 节假日信息
        ScheduleCountRole  // 新增日程数量角色
    };
    // 必需实现的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // 自定义方法
    void setCurrentDate(const QDate &date) ;
    QDate dateFromIndex(const QModelIndex &index) const;


    // 添加高亮支持
    void setHighlightedDates(const QSet<QDate> &dates);
//sssssssssssssss
    // 添加以下方法
    void setSchedules(const QMap<QDate, QList<ScheduleItem>>* schedules);
    const QMap<QDate, QList<ScheduleItem>>* schedules() const;
signals:
    void modelReset();  // 添加模型重置信号
//eeeeeeeeeeeee
protected:
    QSet<QDate> m_highlightedDates;  // 高亮日期集合

private:
    QDate m_currentDate;
    HolidayManager *m_holidayMgr;
    LunarCalculator m_lunarCalc;

    const QMap<QDate, QList<ScheduleItem>>* m_schedules = nullptr; // 使用指针避免数据拷贝
};
#endif // CALENDARMODEL_H
