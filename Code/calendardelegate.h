#ifndef CALENDARDELEGATE_H
#define CALENDARDELEGATE_H

#include <QStyledItemDelegate>
#include <QDate>
#include <QColor>

class CalendarDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CalendarDelegate(QObject *parent = nullptr);

    // 设置当前月份日期
    void setCurrentDate(const QDate &date);

    // 设置热力图最大值
    void setMaxScheduleCount(int max);

    // 绘制函数
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 单元格大小
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

protected:
    // 计算热力图颜色
    QColor heatMapColor(float value, float maxValue) const;

private:
    QDate m_currentDate;      // 当前月份
    int m_maxScheduleCount;   // 热力图最大显示值
};

#endif // CALENDARDELEGATE_H
