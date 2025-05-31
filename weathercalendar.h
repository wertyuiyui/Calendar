#ifndef WEATHERCALENDAR_H
#define WEATHERCALENDAR_H
#pragma once
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QLabel>
#include "calendarview.h"
#include <QPixmap>
#include <QToolButton>
#include <QPushButton>

class WeatherCalendar : public QMainWindow {
    Q_OBJECT
public:
    explicit WeatherCalendar(QWidget *parent = nullptr);

private slots:
    void handleWeatherReply(QNetworkReply *reply);
    void updateWeather();
    void prevMonth();  // 上个月
    void nextMonth();  // 下个月
    void importCalendar();
    void handleCalendarResponse(QNetworkReply *reply);
protected:
    void paintEvent(QPaintEvent *event) override; // 重写绘制事件
    void resizeEvent(QResizeEvent *event) override; // 重写窗口大小改变事件

private:
    void setupApiKey();
    void requestWeather(const QString &city);
    void initUI();
    void initNetwork();
    void applyStyle();

    CalendarView *m_calendarView;
    QNetworkAccessManager *m_weatherManager;
    QString m_apiKey="c0b3c5e1068bdd63c36f6a874308094a";
    QLabel *m_weatherLabel;
    QTimer *m_weatherTimer;
    QPixmap m_background;  // 背景图片
    const QSize MIN_SIZE = QSize(900, 700); // 最小窗口尺寸
    static const int SHADOW_MARGIN = 20;
    QLabel *m_monthLabel;
    QToolButton *m_prevButton;
    QToolButton *m_nextButton;
    QDate m_currentDate;  // 当前显示日期

    void updateMonthDisplay();  // 更新月份显示
    void showWeatherDetail();
    QLabel *m_tempLabel;
    QLabel *m_humidityLabel;
    QLabel *m_windLabel;
    QAction *m_importAction;
    QDialog *m_importDialog;
    QLineEdit *m_urlEdit;
    QPushButton *m_confirmBtn;
    QPushButton *m_importButton;
    void fetchCalendarData(const QUrl &url);
    QAction *m_refreshAction; // 新增刷新按钮
};
#endif // WEATHERCALENDAR_H
