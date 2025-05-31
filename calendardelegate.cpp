#include "calendardelegate.h"
#include <QPainter>
#include <QApplication>
#include <Qdate>
#include "calendarmodel.h"

CalendarDelegate::CalendarDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void CalendarDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // 基础背景绘制
    painter->fillRect(option.rect, QColor(255, 255, 255));  // 强制白色背景

    // ====== 日期有效性判断 ======
    QDate date = index.data(CalendarModel::DateRole).toDate();
    if (!date.isValid()) return;

    // ====== 样式配置 ======
    const bool isCurrentMonth = date.month() == m_currentDate.month();
    const bool isToday = date == QDate::currentDate();
    const QRect contentRect = option.rect.adjusted(5, 5, -5, -5); // 增加边距

    // ====== 当日高亮效果 ======
    if (isToday) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(70, 130, 200, 100));  // 蓝色半透明背景
        painter->drawRoundedRect(contentRect, 8, 8);
        painter->restore();
    }

    // ====== 公历日期文本 ======
    painter->setFont(QFont("Microsoft YaHei", 12, QFont::Medium));
    painter->setPen(isCurrentMonth ? QColor(60, 60, 60) : QColor(180, 180, 180));
    // 在绘制方法中添加周末判断
    if (date.dayOfWeek() == Qt::Saturday || date.dayOfWeek() == Qt::Sunday) {
        painter->setPen(QColor(200, 50, 50));  // 周末红色显示
    }
    painter->drawText(contentRect,
                      Qt::AlignTop | Qt::AlignHCenter,
                      QString::number(date.day()));

    // ====== 农历/节日文本 ======
    QString displayText = index.data(CalendarModel::HolidayRole).toString();
    if (displayText.isEmpty()) {
        displayText = index.data(CalendarModel::LunarRole).toString();
    }

    painter->setFont(QFont("Microsoft YaHei", 9));
    painter->setPen(displayText.contains("节") ? QColor(200, 50, 50) : QColor(150, 150, 150));
    painter->drawText(contentRect,
                      Qt::AlignBottom | Qt::AlignHCenter,
                      displayText);
    // 绘制日程标记
    QString schedule = index.data(CalendarModel::ScheduleRole).toString();
    if(!schedule.isEmpty()) {
        painter->setBrush(QColor(255, 0, 0, 100));
        painter->drawEllipse(option.rect.bottomRight() - QPoint(10,10), 3, 3);
    }
}
void CalendarDelegate::setCurrentDate(const QDate &date)
{
    if (date != m_currentDate) {
        m_currentDate = date;
    }
}
QSize CalendarDelegate::sizeHint(const QStyleOptionViewItem &,
                                 const QModelIndex &) const {
    return QSize(100, 60);  // 自定义单元格大小
}
