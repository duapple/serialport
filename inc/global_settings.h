#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>

typedef struct global_settings{
    bool logToFile;
    bool enableTimeshift;
    QString logPath;
}Global_Settings_t;

class GlobalSettings
{
public:
    Global_Settings_t global_settings;
    Global_Settings_t *settings = &global_settings;
    QJsonObject* globalSettingsToJosn();
    QString globalSettingsToString();
    int StringToGlobalSettings(QString *str);
};

#endif // GLOBAL_SETTINGS_H
