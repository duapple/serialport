#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMutex>

#define LOG_SAVE_PATH  "./"

QString log_file_name;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    QString text;
    switch(type) {
    case QtDebugMsg:
        text = QString("Debug: ");
        break;
    case QtInfoMsg:
        text = QString("Info:  ");
        break;
    case QtWarningMsg:
        text = QString("Warn:  ");
        break;
    case QtCriticalMsg:
        text = QString("Error: ");
        break;
    case QtFatalMsg:
        text = QString("Fatal: ");
        break;
    default:
        text = QString("Debug: ");
    }

    // 设置输出信息格式
    QString context_info = QString("F2:(%1) L:(%2)").arg(QString(context.file)).arg(context.line); // F文件L行数
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("%1 %2  %3").arg(text).arg(strDateTime).arg(msg);
    // 输出信息至文件中（读写、追加形式）
    QFile file(log_file_name);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();
    // 解锁
    mutex.unlock();
}

static void init_log(void)
{
    //注册MessageHandler
    qInstallMessageHandler(outputMessage);
    // log_file_name = CONFIG_PATH+QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").append("-log.txt"));
    log_file_name = LOG_SAVE_PATH + QString("SerialPortAssistan.log");
    QFileInfo file(log_file_name.toStdString().c_str());
    if ( file.size() > 1048576)
    {
        remove(log_file_name.toStdString().c_str());
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    init_log();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "serialPortAssistant_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
