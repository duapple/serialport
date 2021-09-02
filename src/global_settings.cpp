#include "inc/global_settings.h"

using namespace std;

GlobalSettings::GlobalSettings()
{
//    for (int i = 0; i < MAX_USER_CONFIG; i++)
//    {
//        globalConfig.user_config[i].valid = false;
//    }

    foreach (User_Config_t p, config->user_config)
    {

    }
}

GlobalSettings::~GlobalSettings()
{
    delete GSettings;
}

QJsonObject* GlobalSettings::globalSettingsToJosn()
{
    QJsonObject * root = new QJsonObject();

    (*root)["logPath"] = config->global_settings.logPath;
    (*root)["enableTimeshift"] = config->global_settings.enableTimeshift;
    (*root)["logToFile"] = config->global_settings.logToFile;

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

    config->global_settings.logToFile = root["logToFile"].toBool();
    config->global_settings.enableTimeshift = root["enableTimeshift"].toBool();
    config->global_settings.logPath = root["logPath"].toString();

    return 0;
}
