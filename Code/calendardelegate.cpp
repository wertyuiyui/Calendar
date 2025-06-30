#include "calendardelegate.h"
#include <QPainter>
#include <QApplication>
#include <QDate>
#include <QLinearGradient>
#include <cmath>
#include "calendarmodel.h"

CalendarDelegate::CalendarDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
    m_currentDate(QDate::currentDate()),
    m_maxScheduleCount(10)  // 默认最大显示10个日程的热力效果
{
}

void CalendarDelegate::setCurrentDate(const QDate &date)
{
    if (date != m_currentDate) {
        m_currentDate = date;
    }
}

void CalendarDelegate::setMaxScheduleCount(int max)
{
    m_maxScheduleCount = qMax(1, max);  // 确保至少为1
}

QColor CalendarDelegate::heatMapColor(float value, float maxValue) const
{
    // 确保值在0-1范围内
    float ratio = qBound(0.0f, value / maxValue, 1.0f);

    // 从浅蓝色(少)到深红色(多)的渐变
    if (ratio < 0.33f) {
        // 浅蓝到青绿
        return QColor(173, 216, 230, 150); // 浅蓝色
    } else if (ratio < 0.66f) {
        // 青绿到黄色
        return QColor(144, 238, 144, 180); // 浅绿色
    } else {
        // 黄色到红色
        return QColor(255, 99, 71, 210);   // 番茄红
    }
}

void CalendarDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // 基础背景绘制
    painter->fillRect(option.rect, QColor(255, 255, 255, 200));  // 半透明白色背景

    // ====== 日期有效性判断 ======
    QDate date = index.data(CalendarModel::DateRole).toDate();
    if (!date.isValid()) return;

    // ====== 获取日程数量 ======
    int scheduleCount = index.data(CalendarModel::ScheduleCountRole).toInt();

    // ====== 热力图背景 ======
    if (scheduleCount > 0) {
        painter->save();
        painter->setPen(Qt::NoPen);

        // 计算热力颜色
        QColor heatColor = heatMapColor(qMin(scheduleCount, m_maxScheduleCount), m_maxScheduleCount);

        // 根据日程数量设置不同透明度
        int alpha = qMin(200, 100 + scheduleCount * 10);
        heatColor.setAlpha(alpha);

        // 绘制热力背景
        QRect heatRect = option.rect.adjusted(2, 2, -2, -2);
        painter->setBrush(heatColor);

        // 根据日程数量设置不同圆角
        int radius = qMax(3, 8 - scheduleCount / 2);
        painter->drawRoundedRect(heatRect, radius, radius);

        // 在右上角显示日程数量
        if (scheduleCount > 0) {
            painter->setPen(Qt::white);
            painter->setFont(QFont("Microsoft YaHei", 8, QFont::Bold));
            QString countText = QString::number(scheduleCount);
            QRect countRect = QRect(heatRect.right() - 20, heatRect.top(), 20, 20);
            painter->drawText(countRect, Qt::AlignCenter, countText);
        }

        painter->restore();
    }

    // ====== 样式配置 ======
    const bool isCurrentMonth = date.month() == m_currentDate.month();
    const bool isToday = date == QDate::currentDate();
    const QRect contentRect = option.rect.adjusted(5, 5, -5, -5); // 增加边距

    // ====== 当日高亮效果 ======
    if (isToday) {
        painter->save();
        painter->setPen(QPen(QColor(70, 130, 200), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(contentRect, 8, 8);
        painter->restore();
    }

    // ====== 公历日期文本 ======
    painter->setFont(QFont("Microsoft YaHei", 12, isToday ? QFont::Bold : QFont::Medium));
    painter->setPen(isCurrentMonth ?
                        (isToday ? QColor(255, 255, 255) : QColor(60, 60, 60)) :
                        QColor(180, 180, 180));
    painter->drawText(contentRect,
                      Qt::AlignTop | Qt::AlignHCenter,
                      QString::number(date.day()));

    // ====== 农历/节日文本 ======
    QString displayText = index.data(CalendarModel::HolidayRole).toString();
    if (displayText.isEmpty()) {
        displayText = index.data(CalendarModel::LunarRole).toString();
    }

    painter->setFont(QFont("Microsoft YaHei", 9));
    painter->setPen(displayText.contains("节") ?
                        QColor(200, 50, 50) :
                        (isCurrentMonth ? QColor(100, 100, 100) : QColor(180, 180, 180)));
    painter->drawText(contentRect,
                      Qt::AlignBottom | Qt::AlignHCenter,
                      displayText);
}

QSize CalendarDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100, 80);  // 增大单元格大小以更好显示内容
}
