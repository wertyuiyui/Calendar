#ifndef WEATHERCALENDAR_H
#define WEATHERCALENDAR_H
#pragma once
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QLabel>
#include "calendarview.h"
#include <QPixmap>
#include <QToolButton>
//ssss
#include<QPushButton>
#include<QCheckBox>
#include<QListWidget>
#include<QDateTimeEdit>
#include"scheduleitem.h"
#include<QVBoxLayout>
#include<QDialog>
#include<QComboBox>
#include<QFontDatabase>
// 添加头文件
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QNetworkReply>
#include <QProgressBar>
#include<QTableWidget>
// 在头文件顶部添加iCal解析相关声明
#include <QRegularExpression>

// 在头文件顶部添加生日条目结构
struct BirthdayEntry {
    QString name;
    QDate birthday;
    QString note;

    // 计算距离今天的天数
    int daysToToday() const {
        QDate today = QDate::currentDate();
        QDate thisYearBirthday(today.year(), birthday.month(), birthday.day());

        if (thisYearBirthday < today) {
            thisYearBirthday = thisYearBirthday.addYears(1);
        }

        return today.daysTo(thisYearBirthday);
    }
};

// 在FortuneDialog类声明之后添加生日对话框类
class BirthdayDialog : public QDialog {
    Q_OBJECT
public:
    explicit BirthdayDialog(QList<BirthdayEntry>* birthdays, QWidget *parent = nullptr);
    void refreshList();

private slots:
    void addBirthday();
    void editBirthday();
    void deleteBirthday();

private:
    QList<BirthdayEntry>* m_birthdays;
    QTableWidget* m_tableWidget;
};
// 在BirthdayDialog后添加iCal导入对话框
class IcalImportDialog : public QDialog {
    Q_OBJECT
public:
    explicit IcalImportDialog(QMap<QDate, QList<ScheduleItem>>* schedules, QWidget *parent = nullptr);

private slots:
    void importIcal();
    void handleDownloadFinished(QNetworkReply *reply);

private:
    void parseIcalData(const QByteArray &data);

    QMap<QDate, QList<ScheduleItem>>* m_schedules;
    QLineEdit *m_urlEdit;
    QTextEdit *m_logText;
    QProgressBar *m_progressBar;
    QNetworkAccessManager *m_networkManager;
};
//eeeee
// 添加类声明
class FortuneDialog : public QDialog {
    Q_OBJECT
public:
    explicit FortuneDialog(QWidget *parent = nullptr);
    void setFortuneText(const QString &text);
    void showLoading(bool show);

public:
    QTextEdit *m_textEdit;
    QProgressBar *m_progressBar;
    QPushButton *m_retryButton;
};
// 在 FortuneDialog 类声明之后添加新的对话框类
class InspirationDialog : public QDialog {
    Q_OBJECT
public:
    explicit InspirationDialog(const QString &quote, QWidget *parent = nullptr);
};

class ScheduleDialog : public QDialog {
    Q_OBJECT
public:
    explicit ScheduleDialog(QDate date, QMap<QDate, QList<ScheduleItem>>* schedules, QWidget* parent = nullptr);
    void refreshScheduleList();

private slots:
    void addSchedule();
    void deleteSchedule();
    void clearSchedules();

private:
    QComboBox* m_categoryCombo;

private:
    QDate m_currentDate;
    QMap<QDate, QList<ScheduleItem>>* m_schedules;

    // UI组件
    QListWidget* m_scheduleListWidget;
    QLineEdit* m_scheduleInput;
    QDateTimeEdit* m_scheduleTimeInput;
    QCheckBox* m_reminderCheckBox;
};


class WeatherCalendar : public QMainWindow {
    Q_OBJECT
public:
    explicit WeatherCalendar(QWidget *parent = nullptr);
    QDate m_currentDate;  // 当前显示日期
    void testNetworkConnection();
private slots:
    void handleWeatherReply(QNetworkReply *reply);
    void updateWeather();
    void prevMonth();  // 上个月
    void nextMonth();  // 下个月
    void showFortune(); // 新增未名运势槽函数
    void handleFortuneReply(QNetworkReply *reply); // 处理API响应
    void retryFortune();
    void showInspiration();  // 显示励志寄语
protected:
    void paintEvent(QPaintEvent *event) override; // 重写绘制事件
    void resizeEvent(QResizeEvent *event) override; // 重写窗口大小改变事件
private:
    void setupApiKey();
    void requestWeather(const QString &city);
    void initUI();
    void initNetwork();
    void applyStyle();
    QStringList m_inspirationQuotes; // 励志语句库
    void initInspirationQuotes();    // 初始化励志语句库


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
    QLabel* m_weatherIconLabel;



    void updateMonthDisplay();  // 更新月份显示

    //s
private:
    // 日程数据结构
    QMap<QDate, QList<ScheduleItem>> m_schedules;
    QDate m_selectedDate;


    // 新增私有方法
    void initScheduleUI(QVBoxLayout *mainLayout);
    void refreshScheduleList();
    void saveSchedules();
    void loadSchedules();
    void addSchedule();
    void deleteSchedule();
    void handleDateSelected(const QModelIndex &index);
    void clearSchedules();
private:
    // 添加以下成员变量
    int m_maxSchedules = 10; // 热力图最大显示值
public:
    QColor heatMapColor(float value, float maxValue) const;

protected:
    void closeEvent(QCloseEvent *event) override; // 添加关闭事件处理
    //e
private:
    QWidget* m_weatherContainer = nullptr;  // 加这一行，命名建议加 m_ 前缀表示成员变量
    // 右侧工具栏组件
    QWidget *m_rightToolbar;
    QVBoxLayout *m_toolbarLayout;
    QPushButton *m_refreshButton;
    QPushButton *m_scheduleButton;
    QPushButton *m_fortuneButton;
    QPushButton *m_inspirationButton; // 励志寄语按钮
private:
    // 新增成员
    QAction *m_fortuneAction; // 未名运势按钮
    FortuneDialog *m_fortuneDialog; // 运势对话框
    QString m_deepseekApiKey = "sk-84ddef63033f41b4b2d3ccebddc3cd43"; // 替换为您的API密钥
    void logApiResponse(const QByteArray &response,QNetworkReply *reply);
    void verifyDeepSeekApiKey();
    bool m_deepseekApiKeyValid = false;
private:
    // 生日数据
    QList<BirthdayEntry> m_birthdays;

    // 生日按钮
    QPushButton *m_birthdayButton;

private:
    void initBirthdayUI();
    void saveBirthdays();
    void loadBirthdays();

private slots:
    void showBirthdayDialog();
private:
    // iCal导入按钮
    QPushButton *m_icalButton;

private slots:
    void showIcalImportDialog();
};
QByteArray gzipDecompress(const QByteArray &compressedData);

#endif // WEATHERCALENDAR_H
