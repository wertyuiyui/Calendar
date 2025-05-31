#include "calendarview.h"
#include "calendarmodel.h"
#include <QHeaderView>  // 关键头文件包含
#include<QAnimationDriver>
#include <QPropertyAnimation>


CalendarView::CalendarView(QWidget *parent)
    : QTableView(parent) , m_currentDate(QDate::currentDate()){
    // 初始化模型和委托
    setItemDelegate(new CalendarDelegate(this));
    m_model = new CalendarModel(this);
    m_delegate = new CalendarDelegate(this);

    // 视图设置
    setModel(m_model);
    setItemDelegate(m_delegate);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // 必须通过包含QHeaderView才能访问这些方法
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始显示当前月
    m_model->setCurrentDate(QDate::currentDate());

    // 设置列宽自适应
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
void CalendarView::setCurrentDate(const QDate &date) {
    CalendarDelegate *delegate = qobject_cast<CalendarDelegate*>(itemDelegate());
    if (delegate) {
        delegate->setCurrentDate(date);
    }
    if (!date.isValid()) return;

    // 强制设置为当月第一天
    QDate firstDay(date.year(), date.month(), 1);

    if (firstDay != m_currentDate) {
        m_currentDate = firstDay;
        CalendarModel *model = qobject_cast<CalendarModel*>(this->model());
        if (model) {
            model->setCurrentDate(m_currentDate);
        }
        scrollTo(currentIndex());  // 滚动到可见区域
    }
}

QDate CalendarView::currentDate() const {
    return m_currentDate;
}
