#include "holidaymanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// 构造函数（必须显式初始化 QObject）
HolidayManager::HolidayManager(QObject *parent)
    : QObject(parent) // 关键初始化
{
    qDebug() << "HolidayManager initialized";
}

// 加载节假日文件
void HolidayManager::loadFromFile(const QString &path)
{
    qDebug() << "尝试加载节假日文件：" << path;

    QFile file(path);

    // 检查文件是否存在
    if (!file.exists()) {
        qCritical() << "错误：节假日文件不存在！路径：" << path;
        return;
    }

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件：" << file.errorString();
        return;
    }

    // 读取并解析JSON
    QByteArray jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    // 检查解析错误
    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "JSON解析错误：" << parseError.errorString()
            << "，位置：" << parseError.offset;
        return;
    }

    // 解析根对象
    QJsonObject root = doc.object();
    if (!root.contains("2025")) {
        qWarning() << "警告：未找到2025年的节假日数据";
        return;
    }

    // 提取2025年数据
    QJsonObject year2025 = root["2025"].toObject();
    m_holidays.clear();

    // 遍历所有节假日
    for (auto it = year2025.begin(); it != year2025.end(); ++it) {
        QString dateKey = it.key();
        QString holidayName = it.value().toString();

        // 规范化日期格式（确保是4位数字）
        if (dateKey.length() == 3) {
            dateKey = "0" + dateKey;
            qDebug() << "规范化日期格式：" << it.key() << "->" << dateKey;
        }

        // 存入映射表
        m_holidays.insert(dateKey, holidayName);
        qDebug() << "加载节假日：" << dateKey << "->" << holidayName;
    }

    qInfo() << "成功加载" << m_holidays.size() << "个节假日";
}

// 获取节假日名称
QString HolidayManager::getHoliday(const QDate &date) const
{
    // 生成MMdd格式的日期键
    QString key = date.toString("MMdd");

    // 调试输出查询过程
    qDebug() << "查询日期：" << date.toString("yyyy-MM-dd")
             << "，转换键：" << key;

    // 返回节假日名称（无节假日返回空字符串）
    return m_holidays.value(key, "");
}
