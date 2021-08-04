#include "inc/global_settings.h"

using namespace std;

QJsonObject* GlobalSettings::globalSettingsToJosn()
{
    QJsonObject * root = new QJsonObject();

    (*root)["logPath"] = settings->logPath;
    (*root)["enableTimeshift"] = settings->enableTimeshift;
    (*root)["logToFile"] = settings->logToFile;

    QJsonDocument jsonDoc;
    jsonDoc.setObject(*root);

    std::string jsonStr = jsonDoc.toJson().toStdString();

    qInfo() << "global settings: " << jsonStr.c_str();

    return root;
}

QString GlobalSettings::globalSettingsToString()
{
    QJsonDocument jsonDoc;
    jsonDoc.setObject(*globalSettingsToJosn());
    QString jsonStr = QString::fromLocal8Bit(jsonDoc.toJson());

    return jsonStr;
}

int GlobalSettings::StringToGlobalSettings(QString *str)
{
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(str->toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug() << parseJsonErr.errorString();
        return -1;
    }

    QJsonObject root = document.object();

    settings->logToFile = root["logToFile"].toBool();
    settings->enableTimeshift = root["enableTimeshift"].toBool();
    settings->logPath = root["logPath"].toString();

    return 0;
}
