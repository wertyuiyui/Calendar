#ifndef SCHEDULEITEM_H
#define SCHEDULEITEM_H

#include <QDateTime>
#include <QString>
#include <QColor>

class ScheduleItem {
public:
    enum Category {
        None = 0,
        Work,
        Life,
        Study,
        Personal
    };

    ScheduleItem() = default;
    ScheduleItem(const QString& title, const QDateTime& time,
                 bool reminder = false, Category category = None);

    QString toString() const;
    static ScheduleItem fromString(const QString& str);

    // Getter方法
    QString title() const { return m_title; }
    QDateTime time() const { return m_time; }
    bool isReminder() const { return m_reminder; }
    Category category() const { return m_category; }

    // 静态方法声明
    static QString categoryName(Category cat);
    static QColor categoryColor(Category cat);

private:
    QString m_title;
    QDateTime m_time;
    bool m_reminder = false;
    Category m_category = None;
};

#endif // SCHEDULEITEM_H
