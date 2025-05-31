#include "icalparser.h"
#include <QRegularExpression>
#include <QDebug>

IcalParser::IcalParser(QObject *parent) : QObject(parent) {}

QMap<QDate, QString> IcalParser::parseEvents(const QByteArray &data) {
    QMap<QDate, QString> events;

    QString content = QString::fromUtf8(data);
    // 处理多行折叠内容（将折叠行合并）
    content.replace(QRegularExpression("\\r\\n\\s+"), "");
    QStringList lines = content.split("\r\n");
    if (lines.isEmpty()) {
        lines = content.split("\n");
    }

    bool inEvent = false;
    QDate currentDate;
    QString currentSummary;
    QString lastLine;

    for(QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        // 处理多行折叠内容
        if (line.startsWith(" ")) {
            if (!lastLine.isEmpty()) {
                line = lastLine + line.trimmed();
            } else {
                continue;
            }
        }

        if (line.startsWith("BEGIN:VEVENT")) {
            inEvent = true;
            currentDate = QDate();
            currentSummary.clear();
            lastLine = "";
            continue;
        }

        if (line.startsWith("END:VEVENT")) {
            inEvent = false;
            if (currentDate.isValid() && !currentSummary.isEmpty()) {
                events.insert(currentDate, currentSummary);
                qDebug() << "Parsed event:" << currentDate.toString("yyyy-MM-dd") << currentSummary;
            }
            lastLine = "";
            continue;
        }

        if (inEvent) {
            if (line.startsWith("DTSTART")) {
                // 处理带时区的日期格式
                QString value = line.section(':', 1, 1).trimmed();

                // 处理带时区格式：DTSTART;TZID=Asia/Shanghai:20250323T235900
                if (value.contains("TZID")) {
                    value = value.section(':', 1, 1);
                }

                // 处理日期时间格式
                if (value.contains("T")) {
                    value = value.section('T', 0, 0);
                }

                // 处理纯日期格式
                currentDate = parseIcalDate(value);
            }
            else if (line.startsWith("SUMMARY")) {
                currentSummary = line.section(':', 1, 1).trimmed();
            }
            lastLine = line;
        }
    }

    qDebug() << "Total events parsed:" << events.size();
    return events;
}

QDate IcalParser::parseIcalDate(const QString &dateStr) {
    // 移除所有非数字字符
    QString cleanStr = dateStr;
    cleanStr.remove(QRegularExpression("[^0-9]"));

    // 提取日期部分（至少需要8位数字）
    if (cleanStr.length() < 8) {
        qWarning() << "Invalid date string:" << dateStr;
        return QDate();
    }

    QString datePart = cleanStr.left(8);

    int year = datePart.left(4).toInt();
    int month = datePart.mid(4, 2).toInt();
    int day = datePart.mid(6, 2).toInt();

    return QDate(year, month, day);
}
