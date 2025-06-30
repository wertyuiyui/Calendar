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

#include<QVBoxLayout>
#include<QFormLayout>
#include<QComboBox>
#include <QToolBar>  // 添加工具栏头文件
#include <QMouseEvent> // 添加鼠标事件头文件
#include <QFile>        // 添加QFile头文件
#include <QTextStream>  // 添加QTextStream头文件
#include <QDateTime>    // 添加QDateTime头文件
#include <QProxyStyle>
#include <QStyleFactory>
#include <QNetworkProxyFactory>
#include <QSslSocket>
#include <QRandomGenerator>


#include <QCloseEvent> // 添加头文件
#include <QStandardPaths>
#include <QHeaderView>       // 添加 QHeaderView 头文件
#include <QTableWidget>      // 添加 QTableWidget 头文件
#include <QDialogButtonBox>  // 添加 QDialogButtonBox 头文件
#include <QDateEdit>         // 添加 QDateEdit 头文件
#include <QTextEdit>         // 添加 QTextEdit 头文件
#include <QMessageBox>       // 添加 QMessageBox 头文件
#include <algorithm>         // 添加 std::sort 头文件
// 在文件顶部添加必要的头文件
#include <QNetworkReply>
#include <QRegularExpression>
#include <QProgressDialog>
#include <QScrollBar>
#include <QMessageBox>
// 添加必要的头文件
#include <QSslError>
#include <QTimeZone>
#include <zlib.h>

// 修改gzip解压缩函数，添加详细错误处理
QByteArray gzipDecompress(const QByteArray &compressedData)
{
    if (compressedData.isEmpty()) {
        qWarning() << "解压缩错误: 输入数据为空";
        return QByteArray();
    }

    // 准备zlib解压缩流
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    // 使用gzip头部 (16 + MAX_WBITS)
    int initResult = inflateInit2(&zs, 16 + MAX_WBITS);
    if (initResult != Z_OK) {
        qWarning() << "inflateInit2失败，错误代码:" << initResult;
        if (zs.msg) qWarning() << "错误信息:" << zs.msg;
        return QByteArray();
    }

    zs.next_in = (Bytef*)compressedData.data();
    zs.avail_in = compressedData.size();

    int ret;
    QByteArray uncompressed;
    const int BUFFER_SIZE = 32768;
    char outbuffer[BUFFER_SIZE];

    // 解压缩数据
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = BUFFER_SIZE;

        ret = inflate(&zs, Z_NO_FLUSH);

        if (ret == Z_STREAM_ERROR) {
            qWarning() << "gzip解压缩错误: 流错误";
            break;
        }

        int have = BUFFER_SIZE - zs.avail_out;
        if (have > 0) {
            uncompressed.append(outbuffer, have);
        }

        if (ret == Z_DATA_ERROR) {
            qWarning() << "gzip解压缩错误: 数据错误 - 可能是无效的gzip数据";
            break;
        }

        if (ret == Z_MEM_ERROR) {
            qWarning() << "gzip解压缩错误: 内存不足";
            break;
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        qWarning() << "gzip解压缩未完成，错误代码:" << ret;
        if (zs.msg) qWarning() << "错误信息:" << zs.msg;

        // 保存原始数据用于调试
        QFile debugFile("gzip_debug.bin");
        if (debugFile.open(QIODevice::WriteOnly)) {
            debugFile.write(compressedData);
            debugFile.close();
            qWarning() << "原始gzip数据已保存到: gzip_debug.bin";
        }

        return QByteArray();
    }

    qDebug() << "解压缩成功，原始大小:" << compressedData.size()
             << "解压后大小:" << uncompressed.size();
    return uncompressed;
}


void WeatherCalendar::closeEvent(QCloseEvent *event) {
    saveSchedules(); // 保存日程
    saveBirthdays(); // 新增：保存生日数据
    event->accept();
}

// 简单异或加密（生产环境建议使用更安全的算法）
const QString ENCRYPT_KEY = "7d#2@8a3f5b";

WeatherCalendar::WeatherCalendar(QWidget *parent)
    : QMainWindow(parent), m_currentDate(QDate::currentDate())
{
    m_background.load(":/data/background.jpg");
    if (!m_background.load(":/data/background.jpg")) {
        QMessageBox::critical(this, "错误", "无法加载背景图片");
    }
    // 配置SSL支持
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    // 设置网络代理为自动检测
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setMinimumSize(MIN_SIZE);
    initUI();  // 必须先初始化UI
    m_calendarView->setMaxScheduleCount(10);  // 现在m_calendarView已初始化
    loadSchedules();
    initNetwork();
    setupApiKey();
    updateWeather();
    // 添加生日数据加载
    loadBirthdays();
    // 添加励志语句库初始化
    initInspirationQuotes();

    CalendarModel* model = qobject_cast<CalendarModel*>(m_calendarView->model());
    if (model) {
        model->setSchedules(&m_schedules);
    }
}
void WeatherCalendar::initInspirationQuotes()
{
    m_inspirationQuotes.clear();
    m_inspirationQuotes << "裂缝是光照进来的地方，也是新根最有力的生长点。——莱昂纳德·科恩"
                        << "在水泥地上播种的人，终将学会用血泪灌溉。——《卡拉马佐夫兄弟》批注"
                        << "当所有路灯都叛逃黑夜，你便成了自己的银河。——草东没有派对《山海》"
                        << "做一颗逃逸的电子，轨道之外才有照亮黑暗的裂变。——费曼演讲"
                        << "磨钝你的刀锋，世界就敢把脖子伸得更长。——反向哲学"
                        << "苔藓征服悬崖的秘诀：把不可能分解成十万次潮湿的呼吸"
                        << "在标准答案的时代，做个优雅的错别字。——《字体革命》"
                        << "我的加速度，来自所有试图阻挡我的力。——物理悖论"
                        << "当洪流席卷而来，你要做那条溯游产卵的鲑鱼。——生态启示录"
                        << "被折断的铅笔，用断芯在废墟上写生。——广岛原爆资料馆留言"
                        << "真正的风暴眼，是平静到能听见心跳的战场。——登山家梅斯纳尔"
                        << "在磁悬浮的时代做一颗铆钉，把飞翔的梦焊进大地。——工人诗人"
                        << "所有'来不及'，都是胜利对逃亡者的恐吓。——《死亡诗社》弃稿"
                        << "当算法试图定义你，成为那个无法压缩的异常值。——数字抵抗宣言"
                        << "萤火虫的启示：越是黑暗的章节，越要带着光芒断句"
                        << "沙漠教会仙人掌：最锋利的剑藏在谦卑的沉默里"
                        << "卫星的孤独哲学：远离大气层，才能看清风暴的形状"
                        << "在遗忘的图书馆里，做那本被借阅的禁书。——博尔赫斯新解"
                        << "我的锚点不在海底，在尚未命名的星辰。——航海家日记"
                        << "当世界追逐六便士，你在月亮的缺口建天文台"
                        << "蚯蚓的信仰：纵使被踩进地狱，也要把地狱犁成沃土"
                        << "时间是最公平的盗贼，它偷走所有却留下你蜕变的壳"
                        << "在标准化流水线上，做一颗故意跳帧的齿轮";
}
void WeatherCalendar::initUI()
{
    // 创建中央容器
    QWidget *centralWidget = new QWidget;
    centralWidget->setAttribute(Qt::WA_TranslucentBackground);

    // 使用水平布局：左侧为日历内容，右侧为工具栏
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(SHADOW_MARGIN, SHADOW_MARGIN, SHADOW_MARGIN, SHADOW_MARGIN);
    mainLayout->setSpacing(20); // 左右区域间距

    // ====== 左侧日历内容区域 ======
    QWidget *leftContent = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftContent);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10); // 减少空白间距

    // 添加顶部空白区域
    leftLayout->addStretch(1); // 添加顶部空白，将内容向下推

    // ====== 月份控件 ======
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

    // 在leftLayout中添加月份控件
    leftLayout->addWidget(monthWidget, 0, Qt::AlignHCenter);

    // ====== 日历容器 ======
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
    leftLayout->addWidget(calendarContainer, 0);  // 主要空间给日历

    // 添加底部空白
    leftLayout->addStretch(1);

    // 将左侧内容添加到主布局
    mainLayout->addWidget(leftContent, 1); // 左侧占主要空间

    // ====== 右侧工具栏 ======
    m_rightToolbar = new QWidget;
    m_rightToolbar->setObjectName("rightToolbar");
    m_toolbarLayout = new QVBoxLayout(m_rightToolbar);  // 修复：使用成员变量
    m_toolbarLayout->setContentsMargins(15, 15, 15, 15);
    m_toolbarLayout->setSpacing(15);

    // ====== 天气信息显示在工具栏顶部 ======
    QWidget *weatherWidget = new QWidget(m_rightToolbar);
    weatherWidget->setObjectName("weatherWidget");
    QHBoxLayout *weatherLayout = new QHBoxLayout(weatherWidget);
    weatherLayout->setContentsMargins(0, 0, 0, 0);

    // 天气图标
    m_weatherIconLabel = new QLabel(weatherWidget);
    m_weatherIconLabel->setFixedSize(48, 48);
    m_weatherIconLabel->setScaledContents(true);
    m_weatherIconLabel->setObjectName("weatherIconLabel");

    // 天气文字
    m_weatherLabel = new QLabel(weatherWidget);
    m_weatherLabel->setObjectName("weatherLabel");
    m_weatherLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 添加到天气布局
    weatherLayout->addWidget(m_weatherIconLabel);
    weatherLayout->addWidget(m_weatherLabel);
    weatherLayout->setStretch(1, 1); // 文字部分可拉伸

    // 将天气信息添加到工具栏顶部
    m_toolbarLayout->addWidget(weatherWidget);  // 使用成员变量

    // 添加分隔线
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: rgba(200,200,200,100);");
    m_toolbarLayout->addWidget(line);

    // 刷新天气按钮
    m_refreshButton = new QPushButton(tr("刷新天气"), m_rightToolbar);
    m_refreshButton->setObjectName("toolbarButton");
    connect(m_refreshButton, &QPushButton::clicked, this, &WeatherCalendar::updateWeather);
    m_toolbarLayout->addWidget(m_refreshButton);

    // 管理日程按钮
    m_scheduleButton = new QPushButton(tr("管理日程"), m_rightToolbar);
    m_scheduleButton->setObjectName("toolbarButton");
    connect(m_scheduleButton, &QPushButton::clicked, this, [this]() {
        ScheduleDialog dialog(m_selectedDate, &m_schedules, this);
        dialog.exec();
        saveSchedules();
    });
    m_toolbarLayout->addWidget(m_scheduleButton);
    // ====== 新增未名运势按钮 ======
    m_fortuneButton = new QPushButton(tr("未名运势"), m_rightToolbar);
    m_fortuneButton->setObjectName("toolbarButton");
    m_fortuneButton->setIcon(QIcon(":/icons/fortune.png"));
    connect(m_fortuneButton, &QPushButton::clicked, this, &WeatherCalendar::showFortune);
    m_toolbarLayout->addWidget(m_fortuneButton);
    // ====== 在右侧工具栏添加励志寄语按钮 ======
    m_inspirationButton = new QPushButton(tr("励志寄语"), m_rightToolbar);
    m_inspirationButton->setObjectName("toolbarButton");
    m_inspirationButton->setIcon(QIcon(":/icons/inspire.png")); // 添加图标
    connect(m_inspirationButton, &QPushButton::clicked, this, &WeatherCalendar::showInspiration);
    m_toolbarLayout->addWidget(m_inspirationButton);
    // ====== 在右侧工具栏添加生日按钮 ======
    m_birthdayButton = new QPushButton(tr("生日tips"), m_rightToolbar);
    m_birthdayButton->setObjectName("toolbarButton");
    m_birthdayButton->setIcon(QIcon(":/icons/birthday.png")); // 需要添加生日图标
    connect(m_birthdayButton, &QPushButton::clicked, this, &WeatherCalendar::showBirthdayDialog);
    m_toolbarLayout->addWidget(m_birthdayButton);
    // ====== 在右侧工具栏添加iCal导入按钮 ======
    m_icalButton = new QPushButton(tr("iCal导入"), m_rightToolbar);
    m_icalButton->setObjectName("toolbarButton");
    m_icalButton->setIcon(QIcon(":/icons/ical.png")); // 需要添加iCal图标
    connect(m_icalButton, &QPushButton::clicked, this, &WeatherCalendar::showIcalImportDialog);
    m_toolbarLayout->addWidget(m_icalButton);

    // 添加底部空白
    m_toolbarLayout->addStretch(1);

    // 将工具栏添加到主布局右侧
    mainLayout->addWidget(m_rightToolbar, 0); // 固定宽度

    // 设置中央部件
    setCentralWidget(centralWidget);

    // ====== 连接信号槽 ======
    // 月份按钮
    connect(m_prevButton, &QToolButton::clicked, this, &WeatherCalendar::prevMonth);
    connect(m_nextButton, &QToolButton::clicked, this, &WeatherCalendar::nextMonth);

    // 日期选择信号
    connect(m_calendarView, &QTableView::clicked, this, [this](const QModelIndex &index){
        m_selectedDate = index.data(CalendarModel::DateRole).toDate();
    });

    // 初始显示当前月
    updateMonthDisplay();
    m_calendarView->setCurrentDate(QDate::currentDate());

    // 设置日历模型
    CalendarModel* model = qobject_cast<CalendarModel*>(m_calendarView->model());
    if (model) {
        model->setSchedules(&m_schedules);
    }

    // 创建运势对话框（但不显示）
    m_fortuneDialog = new FortuneDialog(this);
    // 应用样式
    applyStyle();
}


void WeatherCalendar::applyStyle()
{
    QString style;

    // ====== 右侧工具栏样式 ======
    style += R"(
        /* 右侧工具栏样式 - 固定宽度 */
        QWidget#rightToolbar {
            background-color: rgba(245, 245, 245, 220);
            border-left: 1px solid rgba(255, 255, 255, 200);
            border-radius: 15px 0 0 15px;
            min-width: 150px;  /* 最小宽度 */
            max-width: 180px; /* 最大宽度 */
        }

        /* 天气信息容器 */
        QWidget#weatherWidget {
            padding: 5px;
        }

        /* 天气文字样式 */
        QLabel#weatherLabel {
            font-size: 12px;
            color: #333333;
            padding-left: 5px;
        }

        /* 工具栏按钮样式 */
        QPushButton#toolbarButton {
            background-color: rgba(240, 240, 240, 180);
            border: 1px solid rgba(200, 200, 200, 150);
            border-radius: 8px;
            padding: 10px 5px;
            color: #333333;
            font: bold 10pt 'Microsoft YaHei';
            min-height: 40px;
        }

        QPushButton#toolbarButton:hover {
            background-color: rgba(230, 230, 230, 200);
            border-color: rgba(180, 180, 180, 180);
        }

        QPushButton#toolbarButton:pressed {
            background-color: rgba(220, 220, 220, 220);
        }

        /* 分隔线样式 */
        QFrame[frameShape="4"] { /* HLine */
            background-color: rgba(200, 200, 200, 100);
            height: 1px;
            margin: 10px 0;
        }
    )";

    // ====== 其他原有样式保持不变 ======
    style +=
        "QWidget#calendarContainer {"
        "   background: rgba(255, 255, 255, 220);"
        "   border-radius: 15px;"
        "}"

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
        "}"

        "QTableView {"
        "   background: #ffffff;"      // 白色背景
        "   color: #333333;"           // 深灰色文字
        "   gridline-color: #e0e0e0;"  // 浅灰色网格线
        "}"

        "QHeaderView::section {"
        "   background: #f8f8f8;"      // 表头浅灰色
        "   color: #666666;"
        "}";
    // 添加运势按钮样式
    style +=
        "QToolBar QToolButton {"
        "   min-width: 90px;"
        "   text-align: left;"
        "   padding-left: 10px;"
        "}"
        "QToolBar QToolButton:hover {"
        "   background: rgba(200, 180, 150, 50);"
        "}";

    // 添加自定义字体
    int fontId = QFontDatabase::addApplicationFont(":/images/ziti.TTF");
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        style += QString(
                     "QLabel#weatherLabel {"
                     "   font-family: '%1';"
                     "   font-size: 12px;"
                     "}").arg(family);
    }

    qApp->setStyleSheet(style);
}

void WeatherCalendar::initNetwork()
{
    m_weatherManager = new QNetworkAccessManager(this);

    // 设置接受gzip编码
    m_weatherManager->setTransferTimeout(30000); // 30秒超时
    connect(m_weatherManager, &QNetworkAccessManager::finished,
            this, &WeatherCalendar::handleWeatherReply);

    // 配置SSL
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    // 设置代理
    QNetworkProxyFactory::setUseSystemConfiguration(true);

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

void WeatherCalendar::handleWeatherReply(QNetworkReply *reply)
{
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

        QString weatherText = QString(tr("%1\n%2°C"))
                                  .arg(desc).arg(temp);
        m_weatherLabel->setText(weatherText);

        // 显示图标
        QString iconPath = ":/images/yin.png";

        if (desc == "阴") iconPath = ":/images/yin.png";
        else if (desc == "多云") iconPath = ":/images/cloudy.png";
        else if (desc == "雨"||desc=="小雨"||desc=="中雨"||desc=="大雨") iconPath = ":/images/rainy.png";
        else if (desc == "雪") iconPath = ":/images/snowy.png";
        else if (desc == "晴") iconPath = ":/images/sunny.png";
        else if (desc == "雷阵雨") iconPath = ":/images/leiyu.png";

        QPixmap icon(iconPath);
        m_weatherIconLabel->setPixmap(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    // 绘制背景
    if (!m_background.isNull()) {
        QPixmap scaled = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        // 更新背景缓存（如果需要）
    }

    // 调整日历大小
    if (m_calendarView) {
        // 使用整数计算
        int maxCalendarHeight = static_cast<int>(height() * 0.7);
        int calendarWidth = static_cast<int>(width() * 0.6);

        // 使用 qMin 比较两个整数
        int calendarSize = qMin(calendarWidth, maxCalendarHeight);

        // 计算日历高度（整数运算）
        int calendarHeight = static_cast<int>(calendarSize * 0.8);

        m_calendarView->setFixedSize(calendarSize, calendarHeight);
    }

    // 确保工具栏宽度合适
    if (m_rightToolbar) {
        m_rightToolbar->setFixedWidth(150); // 固定宽度
    }

    // 更新月份显示
    updateMonthDisplay();
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


//ssssssssssssss

//sssssssssssssssssss
ScheduleDialog::ScheduleDialog(QDate date, QMap<QDate, QList<ScheduleItem>>* schedules, QWidget* parent)
    : QDialog(parent), m_currentDate(date), m_schedules(schedules)
{
    setWindowTitle(tr("日程管理 - %1").arg(date.toString("yyyy-MM-dd")));
    setWindowModality(Qt::WindowModal);
    setMinimumSize(500, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 日程列表
    m_scheduleListWidget = new QListWidget;
    m_scheduleListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // 输入区域
    QFormLayout* formLayout = new QFormLayout;

    // 日程内容
    m_scheduleInput = new QLineEdit;
    m_scheduleInput->setPlaceholderText(tr("输入日程内容"));
    formLayout->addRow(tr("内容:"), m_scheduleInput);

    // 时间选择
    m_scheduleTimeInput = new QDateTimeEdit(QDateTime(date, QTime::currentTime()));
    m_scheduleTimeInput->setDisplayFormat("yyyy-MM-dd HH:mm");
    formLayout->addRow(tr("时间:"), m_scheduleTimeInput);

    // 分类选择
    m_categoryCombo = new QComboBox;
    m_categoryCombo->addItem(tr("无"), ScheduleItem::None);
    m_categoryCombo->addItem(tr("工作"), ScheduleItem::Work);
    m_categoryCombo->addItem(tr("生活"), ScheduleItem::Life);
    m_categoryCombo->addItem(tr("学习"), ScheduleItem::Study);
    m_categoryCombo->addItem(tr("个人"), ScheduleItem::Personal);
    formLayout->addRow(tr("分类:"), m_categoryCombo);

    // 提醒复选框
    m_reminderCheckBox = new QCheckBox(tr("设置提醒"));
    formLayout->addRow("", m_reminderCheckBox);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* addButton = new QPushButton(tr("添加"));
    QPushButton* deleteButton = new QPushButton(tr("删除"));
    QPushButton* clearButton = new QPushButton(tr("清空"));

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);

    // 组装主布局
    mainLayout->addWidget(new QLabel(tr("当日日程:")));
    mainLayout->addWidget(m_scheduleListWidget, 1);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(addButton, &QPushButton::clicked, this, &ScheduleDialog::addSchedule);
    connect(deleteButton, &QPushButton::clicked, this, &ScheduleDialog::deleteSchedule);
    connect(clearButton, &QPushButton::clicked, this, &ScheduleDialog::clearSchedules);

    // 回车键添加日程
    m_scheduleInput->setFocus();
    connect(m_scheduleInput, &QLineEdit::returnPressed, this, &ScheduleDialog::addSchedule);

    refreshScheduleList();

}


void ScheduleDialog::refreshScheduleList() {
    m_scheduleListWidget->clear();
    const auto& schedules = m_schedules->value(m_currentDate);

    for (const auto& item : schedules) {
        QString text = QString("[%1] %2 (%3)")
        .arg(item.time().toString("HH:mm"))
            .arg(item.title())
            .arg(ScheduleItem::categoryName(item.category()));

        QListWidgetItem* listItem = new QListWidgetItem(text);

        // 设置分类颜色和样式
        QColor bgColor = ScheduleItem::categoryColor(item.category());
        listItem->setBackground(bgColor);
        listItem->setForeground(Qt::black);

        // 设置圆角和边距
        listItem->setData(Qt::UserRole+1, QVariant(5)); // 圆角半径
        listItem->setData(Qt::UserRole+2, QVariant(2)); // 边距

        // 提醒事项加粗显示
        if (item.isReminder()) {
            QFont font = listItem->font();
            font.setBold(true);
            listItem->setFont(font);
        }

        // 工具提示
        listItem->setToolTip(QString("分类: %1\n时间: %2\n内容: %3")
                                 .arg(ScheduleItem::categoryName(item.category()))
                                 .arg(item.time().toString("yyyy-MM-dd HH:mm"))
                                 .arg(item.title()));

        m_scheduleListWidget->addItem(listItem);
    }

    // 无日程提示
    if (schedules.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(tr("当天没有日程安排"));
        item->setForeground(Qt::gray);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        m_scheduleListWidget->addItem(item);
    }
}

void ScheduleDialog::addSchedule() {
    QString text = m_scheduleInput->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入日程内容"));
        return;
    }

    ScheduleItem item(
        text,
        m_scheduleTimeInput->dateTime(),
        m_reminderCheckBox->isChecked(),
        static_cast<ScheduleItem::Category>(m_categoryCombo->currentData().toInt())
        );

    // 检查时间是否合法
    if (item.time() < QDateTime::currentDateTime()) {
        QMessageBox::warning(this, tr("警告"), tr("不能添加过去时间的日程"));
        return;
    }

    (*m_schedules)[m_currentDate].append(item);
    refreshScheduleList();
    m_scheduleInput->clear();
    m_scheduleInput->setFocus();
}

void ScheduleDialog::deleteSchedule() {
    int row = m_scheduleListWidget->currentRow();
    if (row >= 0 && row < (*m_schedules)[m_currentDate].size()) {
        (*m_schedules)[m_currentDate].removeAt(row);
        refreshScheduleList();
    } else {
        QMessageBox::warning(this, tr("提示"), tr("请选择要删除的日程"));
    }
}

void WeatherCalendar::saveSchedules() {
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    + "/WeatherCalendarSchedules.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入:" << filePath;
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // 写入文件头
    out << "# Weather Calendar Schedules\n";
    out << "# Format: DATE|TITLE|DATETIME|REMINDER|CATEGORY\n";
    out << "# -------------------------------------------\n";

    // 遍历所有日程
    for (auto it = m_schedules.begin(); it != m_schedules.end(); ++it) {
        const QDate &date = it.key();
        for (const auto &item : it.value()) {
            out << date.toString(Qt::ISODate) << "|"
                << item.title() << "|"
                << item.time().toString(Qt::ISODate) << "|"
                << (item.isReminder() ? "1" : "0") << "|"
                << static_cast<int>(item.category()) << "\n";
        }
    }

    file.close();
    qDebug() << "日程已保存到:" << filePath;

    // 保存后强制更新日历视图
    m_calendarView->setCurrentDate(m_currentDate);

    // 更新热力图最大值
    m_maxSchedules = 0;
    for (const auto& list : m_schedules) {
        if (list.size() > m_maxSchedules) {
            m_maxSchedules = list.size();
        }
    }
    m_calendarView->setMaxScheduleCount(qMax(10, m_maxSchedules));
}

void WeatherCalendar::loadSchedules() {
    // 创建文件路径
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                       + "/WeatherCalendarSchedules.txt";

    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无日程文件或无法打开:" << filePath;
        return;
    }

    m_schedules.clear();
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("#") || line.isEmpty()) continue; // 跳过注释行

        QStringList parts = line.split("|");
        if (parts.size() < 5) continue; // 确保有足够字段

        // 解析字段
        QDate date = QDate::fromString(parts[0], Qt::ISODate);
        QString title = parts[1];
        QDateTime time = QDateTime::fromString(parts[2], Qt::ISODate);
        bool reminder = (parts[3] == "1");
        ScheduleItem::Category category = static_cast<ScheduleItem::Category>(parts[4].toInt());

        // 添加到日程列表
        if (date.isValid()) {
            m_schedules[date].append(ScheduleItem(title, time, reminder, category));
        }
    }

    file.close();
    qDebug() << "从文件加载日程:" << m_schedules.size() << "条记录";

    // 更新日历视图
    if (m_calendarView) {
        CalendarModel* model = qobject_cast<CalendarModel*>(m_calendarView->model());
        if (model) {
            model->setSchedules(&m_schedules);
            m_calendarView->viewport()->update();
        }
    }

    // 计算最大日程数用于热力图
    m_maxSchedules = 0;
    for (const auto& list : m_schedules) {
        if (list.size() > m_maxSchedules) {
            m_maxSchedules = list.size();
        }
    }
    m_calendarView->setMaxScheduleCount(qMax(10, m_maxSchedules));
}

void ScheduleDialog::clearSchedules() {
    if ((*m_schedules)[m_currentDate].isEmpty()) {
        return;
    }

    int ret = QMessageBox::question(this,
                                    tr("确认清空"),
                                    tr("确定要清空%1的所有日程吗？").arg(m_currentDate.toString("yyyy-MM-dd")),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        m_schedules->remove(m_currentDate);
        refreshScheduleList();
    }
}

QColor WeatherCalendar::heatMapColor(float value, float maxValue) const {
    // 确保值在0-1范围内
    float ratio = qBound(0.0f, value / maxValue, 1.0f);

    // 从蓝色(少)到红色(多)的渐变
    if (ratio < 0.5f) {
        return QColor(0, static_cast<int>(255 * ratio * 2), 255); // 蓝到青
    } else {
        return QColor(static_cast<int>(255 * (ratio - 0.5f) * 2), 255,
                      static_cast<int>(255 * (1.0f - ratio) * 2)); // 青到红
    }
}
FortuneDialog::FortuneDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("未名运势");
    setFixedSize(600, 500); // 增大对话框尺寸

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    // 标题
    QLabel *titleLabel = new QLabel("未名运势 - 北京大学专属运势");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #8B4513;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // 分隔线
    QFrame *divider = new QFrame;
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #D2B48C; height: 2px;");
    layout->addWidget(divider);

    // 进度条（加载状态）
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 0); // 无限进度
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 1px solid #D2B48C;"
        "   border-radius: 4px;"
        "   background: #FAF0E6;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #8B4513;"
        "}"
        );
    layout->addWidget(m_progressBar);

    // 文本显示区域
    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "   background-color: #FAF0E6;"
        "   border: 1px solid #D2B48C;"
        "   border-radius: 8px;"
        "   padding: 15px;"
        "   font-family: 'Microsoft YaHei';"
        "   font-size: 14px;"
        "   color: #5a4a30;"
        "}"
        );
    layout->addWidget(m_textEdit, 1); // 添加伸缩因子

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    m_retryButton = new QPushButton("重新获取");
    m_retryButton->setStyleSheet(
        "QPushButton {"
        "   background: #8c7851;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: #a8956e;"
        "}"
        );
    m_retryButton->setVisible(false);

    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: #7a6651;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: #8a7560;"
        "}"
        );

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_retryButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    // 初始状态
    showLoading(true);

    // 连接信号
    connect(closeButton, &QPushButton::clicked, this, &FortuneDialog::accept);
}

void FortuneDialog::setFortuneText(const QString &text) {
    m_textEdit->setText(text);
}

void FortuneDialog::showLoading(bool show) {
    m_progressBar->setVisible(show);
    m_textEdit->setVisible(!show);
    m_retryButton->setVisible(!show);
}

// =============================================
// WeatherCalendar 运势功能实现
// =============================================

// 修改logApiResponse函数，记录更多信息
void WeatherCalendar::logApiResponse(const QByteArray &response, QNetworkReply *reply)
{
    QFile file("deepseek_api.log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "]\n";

        if (reply) {
            out << "URL: " << reply->url().toString() << "\n";
            out << "HTTP方法: " << (reply->operation() == QNetworkAccessManager::PostOperation ? "POST" : "GET") << "\n";
            out << "状态码: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << "\n";
            out << "错误: " << reply->errorString() << "\n";

            // 记录请求头
            out << "请求头:\n";
            QList<QByteArray> headers = reply->request().rawHeaderList();
            foreach (const QByteArray &header, headers) {
                out << header << ": " << reply->request().rawHeader(header) << "\n";
            }
        }

        out << "响应内容:\n";
        if (response.size() > 1000) {
            out << response.left(500) << "\n...\n" << response.right(500);
        } else {
            out << response;
        }
        out << "\n----------------------------------------\n\n";
        file.close();
    }
}

void WeatherCalendar::verifyDeepSeekApiKey() {
    if (m_deepseekApiKey.isEmpty()) {
        m_deepseekApiKeyValid = false;
        return;
    }

    // 使用简单的API测试端点验证密钥
    QNetworkRequest request(QUrl("https://api.deepseek.com/models"));
    request.setRawHeader("Authorization", ("Bearer " + m_deepseekApiKey).toUtf8());

    QNetworkReply* reply = m_weatherManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            m_deepseekApiKeyValid = false;
            qDebug() << "API密钥验证失败:" << reply->errorString();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject() && doc.object().contains("data")) {
            m_deepseekApiKeyValid = true;
            qDebug() << "API密钥验证成功";
        } else {
            m_deepseekApiKeyValid = false;
            qDebug() << "API密钥验证失败：无效的响应";
        }
    });
}

void WeatherCalendar::showFortune() {

    // 显示加载状态
    m_fortuneDialog->showLoading(true);
    m_fortuneDialog->setFortuneText("");
    m_fortuneDialog->show();

    // 获取当前日期
    QDate today = QDate::currentDate();
    QString dateStr = today.toString("yyyy年MM月dd日");
    QString weekday = today.toString("dddd");

    // 构建DeepSeek API请求
    QNetworkRequest request(QUrl("https://api.deepseek.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_deepseekApiKey = "sk-84ddef63033f41b4b2d3ccebddc3cd43";
    request.setRawHeader("Authorization", ("Bearer " + m_deepseekApiKey).toUtf8());
    request.setRawHeader("Accept", "application/json");

    // 构建提示词
    QString prompt = QString(
                         "作为北京大学专属运势分析专家，请为%1（%2）生成一份特色运势报告。"
                         "要求：\n\n"
                         "1. 格式包含：\n"
                         "   - 运势标题（富有诗意，包含北大元素）\n"
                         "   - 幸运颜色\n"
                         "   - 幸运数字\n"
                         "   - 宜做事项（3-5项学术相关）\n"
                         "   - 忌做事项（3-5项学术相关）\n"
                         "   - 学术建议（结合周易卦象）\n"
                         "   - 健康提醒\n\n"
                         "2. 内容要求：\n"
                         "   - 融入未名湖、博雅塔、图书馆等北大元素\n"
                         "   - 结合周易卦象分析（如'今日卦象为乾为天，主刚健有为'）\n"
                         "   - 学术建议具体可行（如'宜复习线性代数'）\n"
                         "   - 健康提醒实用（如'未名湖慢跑30分钟'）\n"
                         "   - 风格幽默风趣，使用学生熟悉的语言\n\n"
                         "3. 输出格式：\n"
                         "【未名运势】标题\n"
                         "✨ 幸运颜色：xx\n"
                         "🔢 幸运数字：xx\n"
                         "✅ 宜：xx、xx、xx\n"
                         "❌ 忌：xx、xx、xx\n"
                         "📚 学术建议：xx\n"
                         "💪 健康提醒：xx\n\n"
                         "详细分析：150-250字的运势解读，包含卦象分析和校园场景建议"
                         ).arg(dateStr, weekday);

    // 构建请求体
    QJsonObject requestBody;
    requestBody["model"] = "deepseek-chat";
    requestBody["temperature"] = 0.7;
    requestBody["max_tokens"] = 2000;
    requestBody["top_p"] = 1.0;
    requestBody["frequency_penalty"] = 0;
    requestBody["presence_penalty"] = 0;

    QJsonArray messages;
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = "你是一位精通周易算法和校园文化的运势分析专家，为北京大学学生提供每日运势预测";
    messages.append(systemMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = prompt;
    messages.append(userMessage);

    requestBody["messages"] = messages;
    requestBody["stream"] = false;

    // 记录请求详情
    qDebug() << "发送DeepSeek API请求:";
    qDebug() << "URL:" << request.url().toString();
    qDebug() << "Headers:" << request.rawHeaderList();
    qDebug() << "Body:" << QJsonDocument(requestBody).toJson(QJsonDocument::Indented);

    // 发送请求
    QNetworkReply *reply = m_weatherManager->post(request, QJsonDocument(requestBody).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleFortuneReply(reply);
    });
}

void WeatherCalendar::handleFortuneReply(QNetworkReply *reply)
{
    reply->deleteLater();
    m_fortuneDialog->showLoading(false);

    // 读取原始响应
    QByteArray rawResponse = reply->readAll();

    // 保存原始响应用于调试
    QFile rawResponseFile("raw_api_response.bin");
    if (rawResponseFile.open(QIODevice::WriteOnly)) {
        rawResponseFile.write(rawResponse);
        rawResponseFile.close();
        qDebug() << "原始API响应已保存到: raw_api_response.bin";
    }

    // 检查内容编码
    QByteArray contentEncodingHeader = reply->rawHeader("Content-Encoding");
    QString contentEncoding = QString::fromLatin1(contentEncodingHeader).toLower();
    qDebug() << "内容编码:" << contentEncoding;

    // 如果是gzip压缩，则解压缩
    if (contentEncoding == "gzip") {
        qDebug() << "检测到gzip压缩响应，尝试解压...";
        QByteArray uncompressed = gzipDecompress(rawResponse);

        if (!uncompressed.isEmpty()) {
            qDebug() << "解压成功! 解压后大小:" << uncompressed.size() << "字节";
            rawResponse = uncompressed;

            // 保存解压后的响应
            QFile uncompressedFile("uncompressed_response.json");
            if (uncompressedFile.open(QIODevice::WriteOnly)) {
                uncompressedFile.write(uncompressed);
                uncompressedFile.close();
                qDebug() << "解压后的响应已保存到: uncompressed_response.json";
            }
        } else {
            qDebug() << "解压失败，使用原始响应";
        }
    }

    // 记录响应
    logApiResponse(rawResponse, reply);

    // 处理空响应
    if (rawResponse.isEmpty()) {
        // 构建详细错误信息
        QString errorMsg = "API返回空响应\n\n";
        errorMsg += "URL: " + reply->url().toString() + "\n";
        errorMsg += "状态码: " + QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) + "\n";
        errorMsg += "内容编码: " + contentEncoding + "\n";
        errorMsg += "原始响应大小: " + QString::number(rawResponse.size()) + " 字节\n";

        // 添加网络诊断信息
        errorMsg += "\n网络诊断:\n";

        errorMsg += "  SSL版本: " + QSslSocket::sslLibraryVersionString() + "\n";
        errorMsg += "  zlib版本: " + QString(zlibVersion()) + "\n";

        // 添加请求详情
        errorMsg += "\n请求头:\n";
        foreach (const QByteArray &header, reply->request().rawHeaderList()) {
            errorMsg += header + ": " + reply->request().rawHeader(header) + "\n";
        }

        errorMsg += "\n响应头:\n";
        foreach (const QNetworkReply::RawHeaderPair &pair, reply->rawHeaderPairs()) {
            errorMsg += pair.first + ": " + pair.second + "\n";
        }

        m_fortuneDialog->setFortuneText(errorMsg);
        return;
    }


    // 尝试解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawResponse, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QString parseErrorMsg = QString("JSON解析错误: %1\n位置: %2")
                                    .arg(parseError.errorString())
                                    .arg(parseError.offset);
        m_fortuneDialog->setFortuneText(parseErrorMsg);
        return;
    }

    QJsonObject root = doc.object();
    QString content;

    // 检查错误响应
    if (root.contains("error")) {
        QJsonObject errorObj = root["error"].toObject();
        QString errorMsg = errorObj["message"].toString("未知错误");
        QString type = errorObj["type"].toString("error");

        QString errorResponse = QString("API错误 (%1):\n%2").arg(type, errorMsg);
        m_fortuneDialog->setFortuneText(errorResponse);
        return;
    }

    // 尝试多种可能的响应格式
    if (root.contains("choices")) {
        QJsonArray choices = root["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject choice = choices[0].toObject();

            // 格式1: 包含"message"对象
            if (choice.contains("message")) {
                QJsonObject message = choice["message"].toObject();
                if (message.contains("content")) {
                    content = message["content"].toString();
                }
            }
            // 格式2: 直接包含"text"字段
            else if (choice.contains("text")) {
                content = choice["text"].toString();
            }
        }
    }
    // 格式3: 直接包含"content"字段
    else if (root.contains("content")) {
        content = root["content"].toString();
    }
    // 硅基流动API格式
    else if (root.contains("data") && root["data"].isObject()) {
        QJsonObject data = root["data"].toObject();
        if (data.contains("content")) {
            content = data["content"].toString();
        }
    }

    if (!content.isEmpty()) {
        // 清理和美化内容
        content = content.trimmed();

        // 添加HTML格式化
        content.replace("【", "<b><font color='#8B4513'>【");
        content.replace("】", "】</font></b>");
        content.replace("✨", "<font color='#FFA500' size='5'>✨</font>");
        content.replace("🔢", "<font color='#1E90FF' size='5'>🔢</font>");
        content.replace("✅", "<font color='#32CD32' size='5'>✅</font>");
        content.replace("❌", "<font color='#FF4500' size='5'>❌</font>");
        content.replace("📚", "<font color='#4169E1' size='5'>📚</font>");
        content.replace("💪", "<font color='#FF6347' size='5'>💪</font>");

        // 添加北大主题样式
        QString styledContent = QString(
                                    "<html>"
                                    "<head>"
                                    "<style>"
                                    "body { font-family: 'Microsoft YaHei'; color: #5a4a30; line-height: 1.6; }"
                                    "h1 { color: #8B4513; font-size: 18px; text-align: center; }"
                                    ".highlight { color: #8B4513; font-weight: bold; }"
                                    ".section { margin-top: 15px; }"
                                    ".symbol { font-size: 16px; vertical-align: middle; margin-right: 5px; }"
                                    "</style>"
                                    "</head>"
                                    "<body>"
                                    "<h1>未名运势 · 北京大学</h1>"
                                    "<div class='content'>%1</div>"
                                    "</body>"
                                    "</html>"
                                    ).arg(content);

        m_fortuneDialog->setFortuneText(styledContent);
    } else {
        QString errorMsg = "无法提取内容:\n";
        errorMsg += "请检查API响应格式是否变更\n\n";
        errorMsg += "原始响应:\n";
        errorMsg += rawResponse.left(500); // 只显示前500个字符

        m_fortuneDialog->setFortuneText(errorMsg);
    }
}

void WeatherCalendar::retryFortune() {
    showFortune();
}
// 实现励志寄语功能
void WeatherCalendar::showInspiration()
{
    if (m_inspirationQuotes.isEmpty()) return;

    // 随机选择一条励志语句
    int index = QRandomGenerator::global()->bounded(m_inspirationQuotes.size());
    QString quote = m_inspirationQuotes[index];

    // 创建并显示对话框
    InspirationDialog *dialog = new InspirationDialog(quote, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

// 实现励志寄语对话框
InspirationDialog::InspirationDialog(const QString &quote, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("励志寄语");
    setFixedSize(500, 300);

    // 设置典雅背景
    setStyleSheet(
        "InspirationDialog {"
        "   background-color: #f7f2e0;"
        "   border: 2px solid #d4c0a1;"
        "   border-radius: 15px;"
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);

    // 添加装饰图标
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(":/icons/quote.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    // 添加分隔线
    QFrame *divider = new QFrame;
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #d4c0a1; height: 2px; margin: 10px 0;");
    layout->addWidget(divider);

    // 添加励志语句
    QLabel *quoteLabel = new QLabel(quote);
    quoteLabel->setWordWrap(true);
    quoteLabel->setAlignment(Qt::AlignCenter);
    quoteLabel->setStyleSheet(
        "QLabel {"
        "   font-family: '楷体';"
        "   font-size: 18px;"
        "   color: #8b5a2b;"
        "   line-height: 1.8;"
        "}"
        );
    layout->addWidget(quoteLabel, 1);

    // 添加关闭按钮
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: #d4c0a1;"
        "   color: #5a3c1e;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   min-width: 100px;"
        "   font: bold 12pt 'Microsoft YaHei';"
        "}"
        "QPushButton:hover {"
        "   background: #c5b090;"
        "}"
        );
    connect(closeButton, &QPushButton::clicked, this, &InspirationDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    // 添加装饰边框
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(3, 3);
    setGraphicsEffect(shadow);
}
// 实现生日对话框
BirthdayDialog::BirthdayDialog(QList<BirthdayEntry>* birthdays, QWidget *parent)
    : QDialog(parent), m_birthdays(birthdays)
{
    setWindowTitle("生日提醒");
    setMinimumSize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建表格
    m_tableWidget = new QTableWidget(0, 4); // 4列：姓名、生日、备注、距离天数
    QStringList headers;
    headers << "姓名" << "生日" << "备注" << "距离天数";
    m_tableWidget->setHorizontalHeaderLabels(headers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);

    // 按钮
    QPushButton *addButton = new QPushButton("添加");
    QPushButton *editButton = new QPushButton("编辑");
    QPushButton *deleteButton = new QPushButton("删除");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);

    layout->addWidget(m_tableWidget);
    layout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked, this, &BirthdayDialog::addBirthday);
    connect(editButton, &QPushButton::clicked, this, &BirthdayDialog::editBirthday);
    connect(deleteButton, &QPushButton::clicked, this, &BirthdayDialog::deleteBirthday);

    refreshList();
}

void BirthdayDialog::refreshList()
{
    // 按距离天数排序
    std::sort(m_birthdays->begin(), m_birthdays->end(),
              [](const BirthdayEntry &a, const BirthdayEntry &b) {
                  return a.daysToToday() < b.daysToToday();
              });

    m_tableWidget->setRowCount(m_birthdays->size());
    for (int i = 0; i < m_birthdays->size(); ++i) {
        const BirthdayEntry &entry = m_birthdays->at(i);
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(entry.name));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(entry.birthday.toString("yyyy-MM-dd")));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(entry.note));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(entry.daysToToday())));

        // 高亮显示即将到来的生日
        if (entry.daysToToday() <= 7) {
            for (int col = 0; col < 4; ++col) {
                QTableWidgetItem *item = m_tableWidget->item(i, col);
                item->setBackground(QColor(255, 240, 200)); // 浅橙色背景
            }
        }
    }
}

void BirthdayDialog::addBirthday()
{
    QDialog dialog(this);
    QFormLayout form(&dialog);

    QLineEdit nameEdit;
    QDateEdit dateEdit;
    dateEdit.setCalendarPopup(true);
    QTextEdit noteEdit;
    noteEdit.setMaximumHeight(100);

    form.addRow("姓名:", &nameEdit);
    form.addRow("生日:", &dateEdit);
    form.addRow("备注:", &noteEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        BirthdayEntry entry;
        entry.name = nameEdit.text().trimmed();
        entry.birthday = dateEdit.date();
        entry.note = noteEdit.toPlainText().trimmed();

        if (!entry.name.isEmpty()) {
            m_birthdays->append(entry);
            refreshList();
        }
    }
}

void BirthdayDialog::editBirthday()
{
    int row = m_tableWidget->currentRow();
    if (row < 0 || row >= m_birthdays->size()) return;

    BirthdayEntry &entry = (*m_birthdays)[row];

    QDialog dialog(this);
    QFormLayout form(&dialog);

    QLineEdit nameEdit(entry.name);
    QDateEdit dateEdit(entry.birthday);
    dateEdit.setCalendarPopup(true);
    QTextEdit noteEdit(entry.note);
    noteEdit.setMaximumHeight(100);

    form.addRow("姓名:", &nameEdit);
    form.addRow("生日:", &dateEdit);
    form.addRow("备注:", &noteEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        entry.name = nameEdit.text().trimmed();
        entry.birthday = dateEdit.date();
        entry.note = noteEdit.toPlainText().trimmed();
        refreshList();
    }
}

void BirthdayDialog::deleteBirthday()
{
    int row = m_tableWidget->currentRow();
    if (row < 0 || row >= m_birthdays->size()) return;

    int ret = QMessageBox::question(this, "确认删除",
                                    "确定要删除" + m_birthdays->at(row).name + "吗？");
    if (ret == QMessageBox::Yes) {
        m_birthdays->removeAt(row);
        refreshList();
    }
}

// 生日数据文件路径
const QString BIRTHDAY_FILE = "birthdays.dat";

// 保存生日数据
void WeatherCalendar::saveBirthdays()
{
    QFile file(BIRTHDAY_FILE);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开生日数据文件进行写入:" << BIRTHDAY_FILE;
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    // 写入文件头
    out << quint32(0xB1DA); // 自定义魔数

    // 写入记录数量
    out << static_cast<quint32>(m_birthdays.size());

    // 写入每条记录
    for (const auto& entry : m_birthdays) {
        out << entry.name;
        out << entry.birthday;
        out << entry.note;
    }

    file.close();
    qDebug() << "生日数据已保存到:" << BIRTHDAY_FILE << "记录数:" << m_birthdays.size();
}


// 加载生日数据
void WeatherCalendar::loadBirthdays()
{
    QFile file(BIRTHDAY_FILE);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qWarning() << "生日数据文件不存在或无法打开:" << BIRTHDAY_FILE;
        return;
    }

    m_birthdays.clear();
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    // 检查文件头
    quint32 magic;
    in >> magic;
    if (magic != 0xB1DA) {
        qWarning() << "无效的生日数据文件格式";
        file.close();
        return;
    }

    // 读取记录数量
    quint32 count;
    in >> count;

    // 读取每条记录
    for (quint32 i = 0; i < count; ++i) {
        BirthdayEntry entry;
        in >> entry.name;
        in >> entry.birthday;
        in >> entry.note;

        // 跳过无效条目
        if (entry.name.isEmpty() || !entry.birthday.isValid()) {
            qWarning() << "跳过无效的生日记录";
            continue;
        }

        m_birthdays.append(entry);
    }

    file.close();
    qDebug() << "从文件加载生日数据:" << m_birthdays.size() << "条记录";
}

// 显示生日对话框
void WeatherCalendar::showBirthdayDialog()
{
    BirthdayDialog dialog(&m_birthdays, this);
    dialog.exec();
    saveBirthdays(); // 保存修改
}
// 实现iCal导入对话框
IcalImportDialog::IcalImportDialog(QMap<QDate, QList<ScheduleItem>>* schedules, QWidget *parent)
    : QDialog(parent), m_schedules(schedules)
{
    setWindowTitle("iCal日历导入");
    setMinimumSize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // URL输入
    QLabel *urlLabel = new QLabel(tr("iCal日历URL:"));
    m_urlEdit = new QLineEdit;
    m_urlEdit->setPlaceholderText(tr("https://example.com/calendar.ics"));

    // 示例URL（可帮助用户理解格式）
    QLabel *exampleLabel = new QLabel(tr("示例: https://course.pku.edu.cn/webapps/calendar/calendarFeed/.../learn.ics"));
    exampleLabel->setStyleSheet("color: #666666; font-size: 10pt;");

    // 日志显示
    QLabel *logLabel = new QLabel(tr("导入日志:"));
    m_logText = new QTextEdit;
    m_logText->setReadOnly(true);
    m_logText->setStyleSheet("font-family: monospace;");

    // 进度条
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(false);

    // 导入按钮
    QPushButton *importButton = new QPushButton(tr("开始导入"));

    // 布局
    layout->addWidget(urlLabel);
    layout->addWidget(m_urlEdit);
    layout->addWidget(exampleLabel);
    layout->addSpacing(10);
    layout->addWidget(logLabel);
    layout->addWidget(m_logText, 1);
    layout->addWidget(m_progressBar);
    layout->addWidget(importButton);

    // 网络管理器
    m_networkManager = new QNetworkAccessManager(this);

    // 连接信号
    connect(importButton, &QPushButton::clicked, this, &IcalImportDialog::importIcal);

    // 添加日志
    m_logText->append(tr("准备导入iCal日历..."));
    m_logText->append(tr("请确保URL是公开可访问的iCal日历地址"));
}

void IcalImportDialog::importIcal()
{
    QString urlStr = m_urlEdit->text().trimmed();
    if (urlStr.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请输入iCal日历URL"));
        return;
    }

    QUrl url(urlStr);
    if (!url.isValid()) {
        QMessageBox::warning(this, tr("错误"), tr("URL格式无效"));
        return;
    }

    m_logText->append(tr("\n开始下载日历数据..."));
    m_logText->append(tr("URL: ") + url.toString());

    // 显示进度条
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);

    // 发起网络请求
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);

    // 连接进度信号
    connect(reply, &QNetworkReply::downloadProgress,
            [this](qint64 bytesReceived, qint64 bytesTotal) {
                if (bytesTotal > 0) {
                    int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
                    m_progressBar->setValue(percent);
                }
            });

    // 连接完成信号
    connect(reply, &QNetworkReply::finished,
            this, [this, reply]() { handleDownloadFinished(reply); });
}

void IcalImportDialog::handleDownloadFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    m_progressBar->setVisible(false);

    if (reply->error() != QNetworkReply::NoError) {
        m_logText->append(tr("\n下载失败: ") + reply->errorString());
        QMessageBox::critical(this, tr("错误"), tr("下载日历数据失败: ") + reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();
    if (data.isEmpty()) {
        m_logText->append(tr("\n错误: 下载的数据为空"));
        QMessageBox::warning(this, tr("错误"), tr("下载的数据为空"));
        return;
    }

    m_logText->append(tr("\n下载成功! 数据大小: %1 字节").arg(data.size()));
    m_logText->append(tr("开始解析iCal数据..."));

    parseIcalData(data);
}

void IcalImportDialog::parseIcalData(const QByteArray &data)
{
    QString icalData = QString::fromUtf8(data);
    int eventsAdded = 0;
    int eventsSkipped = 0;

    // 使用正则表达式匹配VEVENT块
    QRegularExpression eventRegex(
        "BEGIN:VEVENT(.+?)END:VEVENT",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption
        );

    QRegularExpressionMatchIterator it = eventRegex.globalMatch(icalData);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString eventBlock = match.captured(1);

        // 提取事件信息
        QRegularExpression dtStartRegex("DTSTART(?:;.+?)?:([0-9TZ]+)");
        QRegularExpression dtEndRegex("DTEND(?:;.+?)?:([0-9TZ]+)");
        QRegularExpression summaryRegex("SUMMARY:(.+)");

        QRegularExpressionMatch dtStartMatch = dtStartRegex.match(eventBlock);
        QRegularExpressionMatch dtEndMatch = dtEndRegex.match(eventBlock);
        QRegularExpressionMatch summaryMatch = summaryRegex.match(eventBlock);

        if (!dtStartMatch.hasMatch() || !summaryMatch.hasMatch()) {
            eventsSkipped++;
            continue;
        }

        QString dtStartStr = dtStartMatch.captured(1);
        QString dtEndStr = dtEndMatch.hasMatch() ? dtEndMatch.captured(1) : dtStartStr;
        QString summary = summaryMatch.captured(1).trimmed();

        // 转换时间格式 (处理带Z的UTC时间和本地时间)
        QDateTime startTime, endTime;

        if (dtStartStr.contains('Z')) {
            // UTC时间格式: yyyyMMddTHHmmssZ
            startTime = QDateTime::fromString(dtStartStr, "yyyyMMddTHHmmss'Z'");
            startTime.setTimeSpec(Qt::UTC);
            startTime = startTime.toLocalTime();
        } else {
            // 本地时间格式: yyyyMMddTHHmmss
            startTime = QDateTime::fromString(dtStartStr, "yyyyMMddTHHmmss");
            if (!startTime.isValid()) {
                // 尝试不带秒的格式
                startTime = QDateTime::fromString(dtStartStr, "yyyyMMddTHHmm");
            }
        }

        if (dtEndStr.contains('Z')) {
            endTime = QDateTime::fromString(dtEndStr, "yyyyMMddTHHmmss'Z'");
            endTime.setTimeSpec(Qt::UTC);
            endTime = endTime.toLocalTime();
        } else {
            endTime = QDateTime::fromString(dtEndStr, "yyyyMMddTHHmmss");
            if (!endTime.isValid()) {
                endTime = QDateTime::fromString(dtEndStr, "yyyyMMddTHHmm");
            }
        }

        // 如果无法解析时间，跳过此事件
        if (!startTime.isValid()) {
            m_logText->append(tr("  - 跳过事件 [无效时间]: %1").arg(summary));
            eventsSkipped++;
            continue;
        }

        // 创建日程项
        ScheduleItem item(
            summary,
            startTime,
            false, // 默认不设置提醒
            ScheduleItem::Study // 默认分类为学习
            );

        // 添加到日程
        QDate date = startTime.date();
        (*m_schedules)[date].append(item);
        eventsAdded++;

        // 添加到日志
        m_logText->append(tr("  + 添加事件: %1").arg(summary));
        m_logText->append(tr("     日期: %1").arg(date.toString("yyyy-MM-dd")));
        m_logText->append(tr("     时间: %1").arg(startTime.toString("HH:mm")));
    }

    // 添加总结信息
    m_logText->append(tr("\n解析完成!"));
    m_logText->append(tr("成功添加事件: %1").arg(eventsAdded));
    m_logText->append(tr("跳过事件: %1").arg(eventsSkipped));

    // 滚动到底部
    QScrollBar *scrollbar = m_logText->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    if (eventsAdded > 0) {
        QMessageBox::information(this, tr("成功"),
                                 tr("已成功导入 %1 个日程事件").arg(eventsAdded));
        accept(); // 关闭对话框
    } else {
        QMessageBox::warning(this, tr("警告"),
                             tr("未找到可导入的事件，或事件格式不支持"));
    }
}

// 显示iCal导入对话框
void WeatherCalendar::showIcalImportDialog()
{
    IcalImportDialog dialog(&m_schedules, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 保存并更新日历
        saveSchedules();
        m_calendarView->setCurrentDate(m_currentDate);
    }
}
