#include "scheduleitem.h"
#include <QMap>

ScheduleItem::ScheduleItem(const QString& title, const QDateTime& time,
                           bool reminder, Category category)
    : m_title(title), m_time(time), m_reminder(reminder), m_category(category) {}

QString ScheduleItem::toString() const {
    return QString("%1|%2|%3|%4")
    .arg(m_title)
        .arg(m_time.toString(Qt::ISODate))
        .arg(m_reminder ? "1" : "0")
        .arg(static_cast<int>(m_category));
}

ScheduleItem ScheduleItem::fromString(const QString& str) {
    QStringList parts = str.split("|");
    Category cat = None;
    if (parts.size() > 3) {
        cat = static_cast<Category>(parts.value(3).toInt());
    }
    return ScheduleItem(
        parts.value(0),
        QDateTime::fromString(parts.value(1), Qt::ISODate),
        parts.value(2) == "1",
        cat
        );
}

QString ScheduleItem::categoryName(Category cat) {
    static const QMap<Category, QString> names = {
        {Work, "工作"},
        {Life, "生活"},
        {Study, "学习"},
        {Personal, "个人"},
        {None, "无"}
    };
    return names.value(cat, "无");
}

QColor ScheduleItem::categoryColor(Category cat) {
    static const QMap<Category, QColor> colors = {
        {Work, QColor(255, 230, 230)},    // 浅红
        {Life, QColor(230, 255, 230)},    // 浅绿
        {Study, QColor(230, 230, 255)},   // 浅蓝
        {Personal, QColor(255, 255, 200)},// 浅黄
        {None, Qt::white}                // 白色
    };
    return colors.value(cat, Qt::white);
}
