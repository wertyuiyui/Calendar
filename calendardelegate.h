#ifndef CALENDARDELEGATE_H
#define CALENDARDELEGATE_H
#pragma once
#include <QStyledItemDelegate>
#include<QDate>

class CalendarDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CalendarDelegate(QObject *parent = nullptr);
    void setCurrentDate(const QDate &date);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
protected:
    QDate m_currentDate;
};
#endif // CALENDARDELEGATE_H
