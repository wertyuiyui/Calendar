#include "mainwindow2.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QNetworkReply>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);

    // 初始化数据库（SQLite）
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("calendar.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "无法打开数据库!");
    }

    // 创建事件表
    QSqlQuery query;
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS calendar_events ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "summary TEXT,"
            "start_time TEXT,"
            "end_time TEXT,"
            "description TEXT)"
            )) {
        QMessageBox::critical(this, "Error", "数据库初始化失败!");
    }

    // 连接按钮点击信号
    connect(ui->importButton, &QPushButton::clicked, this, &MainWindow::onImportClicked);
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
}

void MainWindow::onImportClicked() {
    QString url = ui->urlInput->text();
    if (url.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入 ICS URL");
        return;
    }

    // 显示进度条（可选）
    ui->progressBar->setValue(0);

    // 发起网络请求
    QNetworkRequest request(QUrl(url));
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
        reply->deleteLater();
    });
}

void MainWindow::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "错误", "网络请求失败: " + reply->errorString());
        return;
    }

    QString icsData = QString::fromUtf8(reply->readAll());
    parseIcsData(icsData);
}

void MainWindow::parseIcsData(const QString &icsData) {
    QVector<IcsEvent> events;
    QString lines = icsData;

    // 简单 ICS 解析（可根据需要扩展）
    bool inEvent = false;
    IcsEvent currentEvent;
    QStringList linesList = lines.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : linesList) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine == "BEGIN:VEVENT") {
            inEvent = true;
            currentEvent = IcsEvent();
        } else if (trimmedLine == "END:VEVENT") {
            if (inEvent) {
                events.append(currentEvent);
                inEvent = false;
            }
        } else if (inEvent) {
            if (line.startsWith("SUMMARY:")) {
                currentEvent.summary = line.mid(8).trimmed();
            } else if (line.startsWith("DTSTART:")) {
                currentEvent.startTime = line.mid(8).trimmed();
            } else if (line.startsWith("DTEND:")) {
                currentEvent.endTime = line.mid(6).trimmed(); // 兼容无时间的情况
            } else if (line.startsWith("DESCRIPTION:")) {
                currentEvent.description = line.mid(12).trimmed();
            }
        }
    }

    storeToDatabase(events);
}

void MainWindow::storeToDatabase(const QVector<IcsEvent> &events) {
    QSqlQuery query;
    int successCount = 0;

    for (const auto &event : events) {
        query.prepare(
            "INSERT INTO calendar_events (summary, start_time, end_time, description) "
            "VALUES (:summary, :start_time, :end_time, :description)"
            );
        query.bindValue(":summary", event.summary);
        query.bindValue(":start_time", event.startTime);
        query.bindValue(":end_time", event.endTime);
        query.bindValue(":description", event.description);

        if (query.exec()) {
            successCount++;
        } else {
            ui->logOutput->append("插入失败: " + query.lastError().text());
        }
    }

    ui->logOutput->append(QString("成功导入 %1 个事件").arg(successCount));
    QMessageBox::information(this, "完成", QString("成功导入 %1 个事件").arg(successCount));
}
