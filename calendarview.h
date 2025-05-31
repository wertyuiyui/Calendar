#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H
#pragma once
#include <QTableView>
#include "calendarmodel.h"
#include "calendardelegate.h"

class CalendarView : public QTableView {
    Q_OBJECT
public:
    explicit CalendarView(QWidget *parent = nullptr);
    void setCurrentDate(const QDate &date);
    QDate currentDate() const;

private:
    CalendarModel *m_model;
    CalendarDelegate *m_delegate;
    QDate m_currentDate;
};
#endif // CALENDARVIEW_H
