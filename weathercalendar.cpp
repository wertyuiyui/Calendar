#include "weathercalendar.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QInputDialog>
#include <QSettings>
#include <QHBoxLayout>
#include <QJsonDocument>  // 新增
#include <QJsonObject>    // 新增
#include <QJsonArray>     // 新增
#include <QInputDialog>   // 新增
#include <QSettings>      // 新增
#include <QTimer>         // 新增
#include <QMessageBox>    // 新增
#include <QUrlQuery>      // 新增
#include <QVBoxLayout>    // 新增
#include <QPainter>
#include <QSpacerItem>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QToolBar>
#include "icalparser.h"
#include <QPushButton>


// 简单异或加密（生产环境建议使用更安全的算法）
const QString ENCRYPT_KEY = "7d#2@8a3f5b";

WeatherCalendar::WeatherCalendar(QWidget *parent)
    : QMainWindow(parent) ,m_currentDate(QDate::currentDate()){
    // 加载背景图（路径需要根据实际修改）
    m_background.load(":/data/background.jpg");
    // 在构造函数中添加图片加载检查
    if(!m_background.load(":/data/background.jpg")) {
        QMessageBox::critical(this, "错误", "无法加载背景图片");
    }
    // 设置窗口最小尺寸
    setMinimumSize(MIN_SIZE);


    // 初始化API密钥
    // 初始化UI组件
    initUI();
    initNetwork();
    setupApiKey();
    updateWeather();
}
void WeatherCalendar::initUI()
{
    QToolBar *toolbar = new QToolBar("功能栏", this);
    addToolBar(Qt::RightToolBarArea, toolbar);  // 初始位置在右侧
    toolbar->setMovable(true);  // 保持可拖动特性
    // 主窗口设置
    setMinimumSize(MIN_SIZE);
    setWindowTitle("智能天气日历");

    // 创建中央容器
    QWidget *centralWidget = new QWidget;
    centralWidget->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(SHADOW_MARGIN, SHADOW_MARGIN, SHADOW_MARGIN, SHADOW_MARGIN);

    // 添加上方空白
    mainLayout->addStretch(2);
    // 左侧控制面板



    // 日历容器（带阴影效果）
    QWidget *calendarContainer = new QWidget;
    calendarContainer->setObjectName("calendarContainer");
    QHBoxLayout *calendarHLayout = new QHBoxLayout(calendarContainer);
    calendarHLayout->setContentsMargins(0, 0, 0, 0);

    m_calendarView = new CalendarView;
    m_calendarView->setObjectName("calendarView");
    calendarHLayout->addStretch();
    calendarHLayout->addWidget(m_calendarView);
    calendarHLayout->addStretch();

    // 添加日历区域
    mainLayout->addWidget(calendarContainer, 5);  // 占50%高度

    // 天气信息容器
    QWidget *weatherContainer = new QWidget;
    weatherContainer->setObjectName("weatherContainer");
    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherContainer);
    weatherLayout->setContentsMargins(0, 20, 0, 0);

    m_weatherLabel = new QLabel;
    m_weatherLabel->setObjectName("weatherLabel");
    m_weatherLabel->setAlignment(Qt::AlignCenter);
    weatherLayout->addWidget(m_weatherLabel);

    // 添加天气区域
    mainLayout->addWidget(weatherContainer, 3);  // 占30%高度

    // 设置中央部件
    setCentralWidget(centralWidget);
    QWidget *monthWidget = new QWidget;
    QHBoxLayout *monthLayout = new QHBoxLayout(monthWidget);
    monthLayout->setContentsMargins(0, 0, 0, 0);

    // 上个月按钮
    m_prevButton = new QToolButton;
    m_prevButton->setObjectName("monthButton");
    m_prevButton->setIcon(QIcon(":/icons/left_arrow.png"));
    m_prevButton->setIconSize(QSize(24, 24));
    m_prevButton->setFixedSize(32, 32);

    // 月份显示
    m_monthLabel = new QLabel;
    m_monthLabel->setObjectName("monthLabel");
    m_monthLabel->setAlignment(Qt::AlignCenter);

    // 下个月按钮
    m_nextButton = new QToolButton;
    m_nextButton->setObjectName("monthButton");
    m_nextButton->setIcon(QIcon(":/icons/right_arrow.png"));
    m_nextButton->setIconSize(QSize(24, 24));
    m_nextButton->setFixedSize(32, 32);

    // 添加到布局
    monthLayout->addWidget(m_prevButton);
    monthLayout->addWidget(m_monthLabel, 1);  // 中间部分扩展
    monthLayout->addWidget(m_nextButton);

    // 在mainLayout中添加月份控件（在日历控件之前）
    mainLayout->addWidget(monthWidget, 0, Qt::AlignHCenter);
    mainLayout->addStretch(2);  // 调整空白比例
    mainLayout->addWidget(calendarContainer, 5);

    // 连接信号槽
    connect(m_prevButton, &QToolButton::clicked, this, &WeatherCalendar::prevMonth);
    connect(m_nextButton, &QToolButton::clicked, this, &WeatherCalendar::nextMonth);

    // 初始化显示
    updateMonthDisplay();

    // 应用样式
    applyStyle();

    m_importAction = new QAction(QIcon(":/icons/import.png"), tr("导入iCalendar日程"), this);
    connect(m_importAction, &QAction::triggered, this, &WeatherCalendar::importCalendar);
    toolbar->addAction(m_importAction);

    m_refreshAction = new QAction(QIcon(":/icons/refresh.png"), "刷新天气", this);
    toolbar->addAction(m_refreshAction);
    connect(m_refreshAction, &QAction::triggered, this, &WeatherCalendar::updateWeather);
}

void WeatherCalendar::applyStyle()
{
    // 窗口阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(5, 5);
    centralWidget()->setGraphicsEffect(shadow);

    // 样式表
    QString style =
        "QToolBar {"
        "   background: transparent;"    // 完全透明背景
        "   border: none;"               // 去除边框
        "   spacing: 5px;"               // 按钮间距
        "}"
        "QToolButton {"
        "   color: black;"               // 文字颜色
        "   background: rgba(240,240,240,150);"  // 半透明背景
        "   border: 1px solid rgba(200,200,200,100);"
        "   border-radius: 4px;"
        "   padding: 5px 10px;"
        "}"

                    // 悬停效果
        "QToolButton:hover {"
        "   background: rgba(224,224,224,180);"
        "}"
        "QWidget#calendarContainer {"
        "   background: rgba(255, 255, 255, 220);"
        "   border-radius: 15px;"
        "}"

        "QWidget#weatherContainer {"
        "   background: rgba(255, 255, 255, 180);"
        "   border-radius: 10px;"
        "   margin: 0 20%;"
        "}"

        "QLabel#weatherLabel {"
        "   font-size: 18px;"
        "   color: #333333;"
        "   padding: 10px;"
        "}";
    style +=
        "QToolButton#monthButton {"
        "   background: rgba(255,255,255,0.8);"
        "   border: 1px solid #cccccc;"
        "   border-radius: 16px;"
        "}"
        "QToolButton#monthButton:hover {"
        "   background: rgba(255,255,255,0.9);"
        "   border-color: #999999;"
        "}"
        "QLabel#monthLabel {"
        "   font-size: 18px;"
        "   color: #2c3e50;"
        "   margin: 0 15px;"
        "   min-width: 150px;"
        "}";
    style +=
        "QTableView {"
        "   background: #ffffff;"      // 白色背景
        "   color: #333333;"           // 深灰色文字
        "   gridline-color: #e0e0e0;"  // 浅灰色网格线
        "}"

        "QHeaderView::section {"
        "   background: #f8f8f8;"      // 表头浅灰色
        "   color: #666666;"
        "}";
    qApp->setStyleSheet(style);
}

void WeatherCalendar::initNetwork()
{
    m_weatherManager = new QNetworkAccessManager(this);
    connect(m_weatherManager, &QNetworkAccessManager::finished,
            this, &WeatherCalendar::handleWeatherReply);

    m_weatherTimer = new QTimer(this);
    m_weatherTimer->start(3600000); // 每小时更新
    connect(m_weatherTimer, &QTimer::timeout, this, &WeatherCalendar::updateWeather);
}

void WeatherCalendar::setupApiKey() {
    QSettings settings;
    QByteArray encrypted = settings.value("apiKey").toByteArray();

    if (!encrypted.isEmpty()) {
        // 解密密钥
        QByteArray decrypted = QByteArray::fromBase64(encrypted);
        for(int i=0; i<decrypted.size(); ++i) {
            decrypted[i] = decrypted[i] ^ ENCRYPT_KEY.at(i % ENCRYPT_KEY.size()).toLatin1();
        }
        m_apiKey = QString::fromUtf8(decrypted);
        return;
    }

    // 首次输入密钥
    bool ok;
    QString input = QInputDialog::getText(this,
                                          tr("API密钥"),
                                          tr("请输入OpenWeatherMap API密钥:"),
                                          QLineEdit::Normal,
                                          "", &ok);
    if (ok && !input.isEmpty()) {
        // 加密存储
        QByteArray toEncrypt = input.toUtf8();
        for(int i=0; i<toEncrypt.size(); ++i) {
            toEncrypt[i] = toEncrypt[i] ^ ENCRYPT_KEY.at(i % ENCRYPT_KEY.size()).toLatin1();
        }
        settings.setValue("apiKey", toEncrypt.toBase64());
        m_apiKey = input;
    } else {
        QMessageBox::warning(this,
                             tr("警告"),
                             tr("天气功能需要有效的API密钥"));
    }
}

void WeatherCalendar::requestWeather(const QString &city) {
    if (m_apiKey.isEmpty()) return;

    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;
    query.addQueryItem("q", city);
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "zh_cn");
    url.setQuery(query);

    m_weatherManager->get(QNetworkRequest(url));
}

void WeatherCalendar::handleWeatherReply(QNetworkReply *reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        m_weatherLabel->setText(tr("天气数据获取失败"));
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        m_weatherLabel->setText(tr("数据解析错误"));
        return;
    }

    QJsonObject root = doc.object();
    if (root.contains("main") && root.contains("weather")) {
        double temp = root["main"].toObject()["temp"].toDouble();
        QString desc = root["weather"].toArray()[0].toObject()["description"].toString();

        QString weatherText = QString(tr("当前天气: %1\n温度: %2°C"))
                                  .arg(desc).arg(temp);
        m_weatherLabel->setText(weatherText);
    }
}

void WeatherCalendar::updateWeather() {
    requestWeather("Beijing"); // 示例使用固定城市
}
// 背景绘制事件
void WeatherCalendar::paintEvent(QPaintEvent* event)
{
    QMainWindow::paintEvent(event);

    QPainter painter(this);

    // 绘制渐变背景
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(255, 255, 255, 50));
    gradient.setColorAt(1, QColor(200, 200, 200, 30));
    painter.fillRect(rect(), gradient);

    // 绘制背景图片
    QPixmap scaled = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawPixmap((width()-scaled.width())/2, (height()-scaled.height())/2, scaled);
}

void WeatherCalendar::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    // 动态调整日历大小
    int baseSize = qMin(width(), height()) * 0.6;
    m_calendarView->setFixedSize(baseSize, baseSize * 0.8);

    // 调整天气字体
    int fontSize = qMax(16, height()/35);
    m_weatherLabel->setStyleSheet(QString("font-size: %1px;").arg(fontSize));
}
void WeatherCalendar::prevMonth()
{
    m_currentDate = m_currentDate.addMonths(-1);

    // 确保日期有效（如从3月30日切到2月会自动调整为28/29日）
    if (!m_currentDate.isValid()) {
        m_currentDate = QDate::currentDate();
    }

    updateMonthDisplay();
    m_calendarView->setCurrentDate(m_currentDate);
}

void WeatherCalendar::nextMonth()
{
    m_currentDate = m_currentDate.addMonths(1);

    // 处理月末边界情况
    if (!m_currentDate.isValid()) {
        m_currentDate = QDate::currentDate();
    }

    updateMonthDisplay();
    m_calendarView->setCurrentDate(m_currentDate);
}

void WeatherCalendar::updateMonthDisplay()
{
    // 添加有效性检查
    if (!m_currentDate.isValid()) {
        m_monthLabel->setText("日期无效");
        return;
    }

    // 使用本地化月份名称
    QString monthName = QLocale(QLocale::Chinese).monthName(
        m_currentDate.month(),
        QLocale::LongFormat
        );

    m_monthLabel->setText(
        QString("%1年%2")
            .arg(m_currentDate.year())
            .arg(monthName)
        );
}
void WeatherCalendar::importCalendar() {
    // 创建导入对话框
    m_importDialog = new QDialog(this);
    m_importDialog->setWindowTitle(tr("导入外部日程"));

    QVBoxLayout *layout = new QVBoxLayout(m_importDialog);

    // URL输入框
    m_urlEdit = new QLineEdit;
    m_urlEdit->setPlaceholderText("请输入iCalendar URL");
    layout->addWidget(m_urlEdit);

    // 确认按钮
    m_confirmBtn = new QPushButton(tr("导入"));
    connect(m_confirmBtn, &QPushButton::clicked, [this](){
        QUrl url(m_urlEdit->text());
        if(url.isValid() && !url.isEmpty()) {
            fetchCalendarData(url);
            m_importDialog->close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("请输入有效的URL"));
        }
    });
    layout->addWidget(m_confirmBtn);

    m_importDialog->exec();
}
void WeatherCalendar::fetchCalendarData(const QUrl &url) {
    QNetworkRequest request(url);
    QNetworkReply *reply = m_weatherManager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply](){
        handleCalendarResponse(reply);
    });
}
// weathercalendar.cpp
void WeatherCalendar::handleCalendarResponse(QNetworkReply *reply) {
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, tr("错误"),
                              tr("无法获取日程数据: %1").arg(reply->errorString()));
        return;
    }
    QByteArray responseData = reply->readAll();
    qDebug() << "原始响应数据：" << responseData.left(500); // 显示前500字节
    IcalParser parser;
    QMap<QDate, QString> events = parser.parseEvents(reply->readAll());
    for(auto it = events.begin(); it != events.end(); ++it) {
        qDebug() << "发现日程：" << it.key().toString("yyyy-MM-dd") << "->" << it.value();
    }
    if(events.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("未找到有效的日程事件"));
        return;
    }

    CalendarModel *model = qobject_cast<CalendarModel*>(m_calendarView->model());
    model->setSchedules(events);

    // 强制刷新视图
    m_calendarView->viewport()->update();
}
